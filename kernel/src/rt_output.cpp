#include <stdio.h>
#include <math.h>

#include "render.h"
#include "codec_manager.h"
#include "items_manager.h"
#include "rt_output.h"
#include "portaudio.h"
 
namespace Playback
{

enum class EOutputState
{
    eNonInited,
    eStop,
    ePlay,
    ePause
};

struct paRenderData
{
    float* left_buf;
    float* right_buf;
    uint32_t samples_n;
    uint32_t samples_counter;
};

static paRenderData* g_data = nullptr;
static PaStream* g_stream = nullptr;
static PaStreamParameters g_parameters;
static EOutputState g_state = EOutputState::eNonInited;

static int paCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    float* out = (float*) outputBuffer;

    if (!g_data)
    {
        return paComplete;
        g_state = EOutputState::eStop;
    }

    if ((g_data->samples_n - g_data->samples_counter) < framesPerBuffer)
    {
        return paComplete;
        g_state = EOutputState::eStop;
    }

    for (int i = 0; i < framesPerBuffer; i++)
    {
        *out++ = g_data->left_buf[g_data->samples_counter];
        *out++ = g_data->right_buf[g_data->samples_counter];

        g_data->samples_counter++;
    }

    return paContinue;
}

static uint32_t calcSessionLength()
{
    uint32_t max_audio_len = 0;

    for (auto track_id : g_session->getTracks())
    {
        Track* track = ItemsManager::getTrack(track_id);

        if (!track)
        {
            return 0;
        }

        for (auto fragment_id : track->getFragments())
        {
            Fragment* fragment = ItemsManager::getFragment(fragment_id);

            if (!fragment)
            {
                return 0;
            }

            uint32_t len = fragment->getCropTo() - fragment->getCropFrom();
            uint32_t overall_len = len + fragment->getTimeOffset();

            if (max_audio_len < overall_len)
            {
                max_audio_len = overall_len;
            }
        }
    }

    return max_audio_len;
}

static status_t mixAudioToOutBuffer(Fragment* fragment, float* out_buf, uint8_t gain, uint8_t level)
{
    Audio* audio = ItemsManager::getAudio(fragment->getAudio());

    if (!audio)
    {
        return 1;
    }

    uint32_t audio_from_smp = g_session->msToSamples(fragment->getCropFrom());
    uint32_t audio_to_smp = g_session->msToSamples(fragment->getCropTo());
    uint32_t audio_len_smp = audio_to_smp - audio_from_smp;
    uint32_t out_buf_from_smp = g_session->msToSamples(fragment->getTimeOffset());

    if (audio_from_smp >= audio_to_smp)
    {
        return 2;
    }

    if (out_buf_from_smp + audio_len_smp > g_session->msToSamples(calcSessionLength()))
    {
        return 3;
    }

    float k = (gain / 10.0 + 1.0) * (level / 100.0);

    for (uint32_t i = 0; i < audio_len_smp; i++)
    {
        out_buf[out_buf_from_smp + i] += audio->getBuffer()[audio_from_smp + i] * k;
    }

    return 0;
}

static std::set<id_t> getTracksWithSolo()
{
    std::set<id_t> solo_tracks_id;

    for (auto track_id : g_session->getTracks())
    {
        Track* track = ItemsManager::getTrack(track_id);

        if (!track)
        {
            return std::set<id_t>();
        }

        if (track->getSolo())
        {
            solo_tracks_id.insert(track_id);
        }
    }

    return solo_tracks_id;
}

static status_t render_selection(uint32_t from_ms, uint32_t to_ms)
{
    LOG_F(INFO, "Starting rendering session from %d to %d", from_ms, to_ms);

    if (!g_session)
    {
        LOG_F(ERROR, "No session loaded");
        return 1;
    }

    // calc session length in samples
    uint32_t ses_len_ms = calcSessionLength();
    uint32_t ses_len_smp = g_session->msToSamples(ses_len_ms);

    LOG_F(INFO, "Total sessiong length in ms: %d", ses_len_ms);

    LOG_F(INFO, "Allocating intermediate buffers and zeroing them");

    // allocate intermediate buffs

    float* left_buf = new float[ses_len_smp];
    float* right_buf = new float[ses_len_smp];

    for (uint32_t i = 0; i < ses_len_smp; i++)
    {
        left_buf[i] = 0;
        right_buf[i] = 0;
    }

    std::set<id_t> solo_tracks = getTracksWithSolo();
    std::set<id_t> tracks_to_mix = !solo_tracks.empty() ? solo_tracks : g_session->getTracks();

    LOG_F(INFO, solo_tracks.empty() ? "There are some solo tracks, mixing only them" :
        "Mixing all available tracks");

    // mix right and left channels
    for (auto track_id : tracks_to_mix)
    {
        Track* track = ItemsManager::getTrack(track_id);

        if (!track)
        {
            LOG_F(ERROR, "Failed to get track with id %d", track_id);
            return 1;
        }

        LOG_F(INFO, "Got track %d: %s in mix", track_id, track->getName().c_str());

        if (track->getMute())
        {
            LOG_F(INFO, "Track is muted, skipping");
            continue;
        }

        for (auto fragment_id : track->getFragments())
        {
            Fragment* fragment = ItemsManager::getFragment(fragment_id);

            if (!fragment)
            {
                LOG_F(ERROR, "Failed to get fragment with id %d", fragment_id);
                return 1;
            }

            LOG_F(INFO, "Got fragment %d with linked audio %d", fragment_id, fragment->getAudio());
            LOG_F(INFO, "Adding linked audio to intermediate buffers");

            if (mixAudioToOutBuffer(fragment, left_buf, track->getGain(), track->getLevel()) != 0 ||
                mixAudioToOutBuffer(fragment, right_buf, track->getGain(), track->getLevel()) != 0)
            {
                LOG_F(ERROR, "Error occured while mixing");
                return 1;
            }
        }
    }

    if (g_data)
    {
        delete[] g_data->left_buf;
        delete[] g_data->right_buf;
        delete g_data;
    }

    g_data = new paRenderData();

    g_data->right_buf = right_buf;
    g_data->left_buf = left_buf;
    g_data->samples_n = ses_len_smp;
    g_data->samples_counter = 0;

    return 0;
}

status_t init()
{
    if (Pa_Initialize() != paNoError)
    {
        Pa_Terminate();
        return 1;
    }

    g_parameters.device = Pa_GetDefaultOutputDevice();

    if (g_parameters.device == paNoDevice)
    {
        fprintf(stderr, "Error: No default output device.\n");
        Pa_Terminate();
        return 1;
    }

    g_parameters.channelCount = 2;
    g_parameters.sampleFormat = paFloat32;
    g_parameters.suggestedLatency = Pa_GetDeviceInfo(g_parameters.device)->defaultLowOutputLatency;
    g_parameters.hostApiSpecificStreamInfo = NULL;

    g_state = EOutputState::eStop;

    return 0;
}

status_t pause()
{
    if (g_state != EOutputState::ePlay)
    {
        return 1;
    }

    if (Pa_StopStream(g_stream) != paNoError)
    {
        Pa_Terminate();
        return 1;
    }

    g_state = EOutputState::ePause;

    return 0;
}

status_t stop()
{
    if (g_state != EOutputState::ePause && g_state != EOutputState::ePlay)
    {
        return 1;
    }

    if (Pa_CloseStream(g_stream) != paNoError)
    {
        Pa_Terminate();
        return 1;
    }

    g_state = EOutputState::eStop;

    return 0;
}

status_t play(uint32_t from_ms, uint32_t to_ms)
{
    if (g_state == EOutputState::eNonInited)
    {
        init();
    }

    if (g_state == EOutputState::eStop)
    {
        if (render_selection(from_ms, to_ms) != 0)
        {
            return 1;
        }
    }

    PaError err = Pa_OpenStream(&g_stream, nullptr, &g_parameters, g_session->getSampleRate(),
                                256, paClipOff, paCallback, nullptr);

    if (err != paNoError)
    {
        Pa_Terminate();
        return 1;
    }

    if (Pa_StartStream(g_stream) != paNoError)
    {
        Pa_Terminate();
        return 1;
    }

    g_state = EOutputState::ePlay;

    return 0;
}

}