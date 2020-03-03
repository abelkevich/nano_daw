#include "render.h"
#include "codec_manager.h"
#include "items_manager.h"

static std::string getFileExt(const std::string& s) 
{
    const size_t i = s.rfind('.', s.length());
    
    if (i != std::string::npos) 
    {
        return(s.substr(i + 1, s.length() - i));
    }

    return std::string();
}

static ms_t calcSessionLength()
{
    ms_t max_audio_len = 0;

    for (const auto track_id: g_session->getTracks())
    {
        const Track *track = ItemsManager::getTrack(track_id);

        if (!track)
        {
            LOG_F(ERROR, "Cannot find track");
            return 0;
        }

        for (const auto fragment_id : track->getFragments())
        {
            const Fragment *fragment = ItemsManager::getFragment(fragment_id);

            if (!fragment)
            {
                LOG_F(ERROR, "Cannot find fragment");
                return 0;
            }

            const ms_t len = fragment->getCropTo() - fragment->getCropFrom();
            const ms_t overall_len = len + fragment->getTimeOffset();

            if (max_audio_len < overall_len)
            {
                max_audio_len = overall_len;
            }
        }
    }

    return max_audio_len;
}

static bool mixAudioToOutBuffer(const Fragment *fragment, std::shared_ptr<float[]> out_buf, const uint8_t gain, const uint8_t level)
{
    const Audio *audio = ItemsManager::getAudio(fragment->getAudio());

    if (!audio)
    {
        LOG_F(ERROR, "Cannot find linked audio");
        return false;
    }

    const smpn_t audio_from_smp = g_session->msToSamples(fragment->getCropFrom());
    const smpn_t audio_to_smp = g_session->msToSamples(fragment->getCropTo());
    const smpn_t audio_len_smp = audio_to_smp - audio_from_smp;
    const smpn_t out_buf_from_smp = g_session->msToSamples(fragment->getTimeOffset());

    if (audio_from_smp >= audio_to_smp)
    {
        LOG_F(ERROR, "Invalid fragment cropping");
        return false;
    }

    if (out_buf_from_smp + audio_len_smp > g_session->msToSamples(calcSessionLength()))
    {
        LOG_F(ERROR, "Fragment out of bounds in session buffer");
        return false;
    }

    const float k = (gain / 10.0f + 1.0f) * (level / 100.0f);
    for (smpn_t smp_offset = 0; smp_offset < audio_len_smp; smp_offset++)
    {
        out_buf[smp_offset + smp_offset] += audio->getBuffer()[smp_offset + smp_offset] * k;
    }

    return true;
}

static std::set<id_t> getTracksWithSolo()
{
    std::set<id_t> solo_tracks_id;

    for (const auto track_id : g_session->getTracks())
    {
        const Track* track = ItemsManager::getTrack(track_id);

        if (!track)
        {
            LOG_F(ERROR, "Cannot find track");
            return std::set<id_t>();
        }

        if (track->getSolo())
        {
            solo_tracks_id.insert(track_id);
        }
    }

    return solo_tracks_id;
}

bool render(const std::string &mix_path)
{
    LOG_F(INFO, "Starting rendering session");
    
    if (!g_session)
    {
        LOG_F(ERROR, "No session loaded");
        return false;
    }

    const ms_t ses_len_ms = calcSessionLength();
    const smpn_t ses_len_smp = g_session->msToSamples(ses_len_ms);
    LOG_F(INFO, "Total sessiong length in ms: %d", ses_len_ms);

    LOG_F(INFO, "Allocating session buffers");
    std::shared_ptr<float[]> left_buf(new float[ses_len_smp], std::default_delete<float[]>());
    std::shared_ptr<float[]> right_buf(new float[ses_len_smp], std::default_delete<float[]>());

    memset(left_buf.get(), 0, ses_len_smp * sizeof(float));
    memset(right_buf.get(), 0, ses_len_smp * sizeof(float));

    const std::set<id_t> solo_tracks = getTracksWithSolo();
    LOG_F(INFO, !solo_tracks.empty() ? "There are some solo tracks, mixing only them" :
                                       "Mixing all available tracks");

    const std::set<id_t> tracks_to_mix = !solo_tracks.empty() ? solo_tracks : g_session->getTracks();

    // mix right and left channels
    for (const auto track_id: tracks_to_mix)
    {
        const Track *track = ItemsManager::getTrack(track_id);

        if (!track)
        {
            LOG_F(ERROR, "Failed to get track with id %d", track_id);
            return false;
        }

        LOG_F(INFO, "Got track %d: %s in mix", track_id, track->getName().c_str());

        if (track->getMute())
        {
            LOG_F(INFO, "Track is muted, skipping");
            continue;
        }

        const auto fragments = track->getFragments();

        LOG_F(INFO, "Track has %d fragments", fragments.size());

        for (const auto fragment_id: fragments)
        {
            const Fragment *fragment = ItemsManager::getFragment(fragment_id);

            if (!fragment)
            {
                LOG_F(ERROR, "Failed to get fragment with id %d", fragment_id);
                return false;
            }

            LOG_F(INFO, "Got fragment %d with linked audio %d", fragment_id, fragment->getAudio());

            if (!mixAudioToOutBuffer(fragment, left_buf, track->getGain(), track->getLevel()) ||
                !mixAudioToOutBuffer(fragment, right_buf, track->getGain(), track->getLevel()))
            {
                LOG_F(ERROR, "Error occured while mixing");
                return false;
            }
        }
    }

    const std::string file_ext = getFileExt(mix_path);
    LOG_F(INFO, "Searching for codec with target file extension: %s", file_ext.c_str());
    const CodecManager::Codec* codec = CodecManager::findCodecByFileExt(file_ext);

    if (!codec)
    {
        LOG_F(ERROR, "Cannot find codec");
        return false;
    }

    float* arr[2] = { left_buf.get(), right_buf.get() };
    codec->saveFile(CodecFileInfo(mix_path, arr, ses_len_smp, 1, g_session->getSampleRate()), 2);

    LOG_F(INFO, "Session rendered!");

    return true;
}