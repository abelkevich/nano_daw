#include <stdio.h>
#include <math.h>

#include "render.h"
#include "codec_manager.h"
#include "sessions_manager.h"
#include "fragments_manager.h"
#include "tracks_manager.h"
#include "audios_manager.h"
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

static status_t render_selection(const id_t session_id, const uint32_t from_ms, const uint32_t to_ms)
{
    LOG_F(INFO, "Starting rendering session from %d to %d", from_ms, to_ms);

    Session* session = SessionsManager::getSession(session_id);

    if (!session)
    {
        LOG_F(ERROR, "No session loaded");
        return 1;
    }

    // calc session length in samples
    uint32_t ses_len_ms = calcSessionLength(session_id);
    uint32_t ses_len_smp = session->msToSamples(ses_len_ms);

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

    std::set<id_t> solo_tracks = getTracksWithSolo(session_id);
    std::set<id_t> tracks_to_mix = !solo_tracks.empty() ? solo_tracks : session->getTracks();

    LOG_F(INFO, solo_tracks.empty() ? "There are some solo tracks, mixing only them" :
                                      "Mixing all available tracks");

    // mix right and left channels
    for (auto track_id : tracks_to_mix)
    {
        Track* track = TracksManager::getTrack(track_id);

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
            Fragment* fragment = FragmentsManager::getFragment(fragment_id);

            if (!fragment)
            {
                LOG_F(ERROR, "Failed to get fragment with id %d", fragment_id);
                return 1;
            }

            LOG_F(INFO, "Got fragment %d with linked audio %d", fragment_id, fragment->getAudio());
            LOG_F(INFO, "Adding linked audio to intermediate buffers");

            if (mixAudioToOutBuffer(fragment_id, left_buf, ses_len_smp, track->getGain(), track->getLevel()) != 0 ||
                mixAudioToOutBuffer(fragment_id, right_buf, ses_len_smp, track->getGain(), track->getLevel()) != 0)
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

status_t play(const id_t session_id, uint32_t from_ms, uint32_t to_ms)
{
    if (g_state == EOutputState::eNonInited)
    {
        init();
    }
    
    if (g_state == EOutputState::eStop)
    {
        if (render_selection(session_id, from_ms, to_ms) != 0)
        {
            return 1;
        }
    }

    Session* session = SessionsManager::getSession(session_id);

    if (!session)
    {
        return 1;
    }

    PaError err = Pa_OpenStream(&g_stream, nullptr, &g_parameters, session->getSampleRate(),
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