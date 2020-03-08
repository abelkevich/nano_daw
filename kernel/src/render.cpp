#include "render.h"
#include "codec_manager.h"
#include "fragments_manager.h"
#include "sessions_manager.h"
#include "audios_manager.h"
#include "tracks_manager.h"
#include "utils.h"
#include <cstring>

ms_t calcSessionLength(const id_t session_id)
{
    LOG_F(INFO, "Calculating session (id: '%d') length in ms", session_id);
    const Session* session = SessionsManager::getSession(session_id);
    if (!session)
    {
        LOG_F(ERROR, "Cannot get session (id: '%d')", session_id);
        return 0;
    }

    ms_t max_audio_len = 0;

    for (const auto track_id: session->getTracks())
    {
        const Track *track = TracksManager::getTrack(track_id);

        if (!track)
        {
            LOG_F(ERROR, "Cannot get track (id: '%d')", track_id);
            return 0;
        }

        for (const auto fragment_id : track->getFragments())
        {
            const Fragment *fragment = FragmentsManager::getFragment(fragment_id);

            if (!fragment)
            {
                LOG_F(ERROR, "Cannot get fragment (id: '%d')", fragment_id);
                return 0;
            }

            const ms_t len = fragment->getCropTo() - fragment->getCropFrom();
            const ms_t overall_len = len + fragment->getTimeOffset();

            LOG_F(INFO, "Fragment (id: %d) has length: '%d' (ms) and upper bound in timeline: '%d' (ms)", 
                        fragment_id, len, overall_len);

            if (max_audio_len < overall_len)
            {
                max_audio_len = overall_len;
            }
        }
    }

    LOG_F(INFO, "Total session length is '%d' (ms)", max_audio_len);
    return max_audio_len;
}

bool mixAudioToOutBuffer(const id_t fragment_id, float* out_buf, const smpn_t ses_len_smp, const uint8_t gain, const uint8_t level)
{
    const Fragment* fragment = FragmentsManager::getFragment(fragment_id);

    if (!fragment)
    {
        LOG_F(ERROR, "Cannot get fragment (id: '%d')", fragment_id);
        return false;
    }

    const Audio *audio = AudiosManager::getAudio(fragment->getAudio());

    if (!audio)
    {
        LOG_F(ERROR, "Cannot get audio (id: '%d')", fragment->getAudio());
        return false;
    }

    const Track* track = TracksManager::getTrack(fragment->getTrack());

    if (!track)
    {
        LOG_F(ERROR, "Cannot get track (id: '%d')", fragment->getTrack());
        return false;
    }

    const Session* session = SessionsManager::getSession(track->getSessionId());

    if (!session)
    {
        LOG_F(ERROR, "Cannot get session (id: '%d')", track->getSessionId());
        return false;
    }

    const smpn_t audio_from_smp = session->msToSamples(fragment->getCropFrom());
    const smpn_t audio_to_smp = session->msToSamples(fragment->getCropTo());
    const smpn_t audio_len_smp = audio_to_smp - audio_from_smp;
    const smpn_t out_buf_from_smp = session->msToSamples(fragment->getTimeOffset());

    LOG_F(INFO, "Audio info: crop from: '%d' (smp), crop to: '%d' (smp), length: '%d' (smp)",
                audio_from_smp, audio_to_smp, audio_len_smp);

    LOG_F(INFO, "Timeline offset: '%d' (smp)", out_buf_from_smp);

    if (audio_from_smp >= audio_to_smp)
    {
        LOG_F(ERROR, "Fragment (id: %d) has invalid cropping", fragment_id);
        return false;
    }

    if (out_buf_from_smp + audio_len_smp > ses_len_smp)
    {
        LOG_F(ERROR, "Fragment (id: '%d') is out of bounds in session (id: '%d') timeline",  fragment_id, session->getId());
        return false;
    }

    const float k = (gain / 10.0f + 1.0f) * (level / 100.0f);
    for (smpn_t smp_offset = 0; smp_offset < audio_len_smp; smp_offset++)
    {
        out_buf[out_buf_from_smp + smp_offset] += audio->getBuffer()[audio_from_smp + smp_offset] * k;
    }

    return true;
}

std::set<id_t> getTracksWithSolo(const id_t session_id)
{
    Session* session = SessionsManager::getSession(session_id);

    if (!session)
    {
        LOG_F(ERROR, "Cannot get session (id: '%d')", session_id);
        return std::set<id_t>();
    }

    std::set<id_t> solo_tracks_id;

    for (const auto track_id : session->getTracks())
    {
        const Track* track = TracksManager::getTrack(track_id);

        if (!track)
        {
            LOG_F(ERROR, "Cannot get track (id: '%d')", track_id);
            return std::set<id_t>();
        }

        if (track->getSolo())
        {
            solo_tracks_id.insert(track_id);
        }
    }

    return solo_tracks_id;
}

bool render(const id_t session_id, const std::string &mix_path)
{
    LOG_F(INFO, "Rendering session (id: '%d')", session_id);
    
    const Session* session = SessionsManager::getSession(session_id);

    if (!session)
    {
        LOG_F(ERROR, "Cannot get session (id: '%d')", session_id);
        return false;
    }

    const ms_t ses_len_ms = calcSessionLength(session_id);
    const smpn_t ses_len_smp = session->msToSamples(ses_len_ms);
    LOG_F(INFO, "Total sessiong length in ms: %d", ses_len_ms);

    std::shared_ptr<float[]> left_buf(new float[ses_len_smp], std::default_delete<float[]>());
    std::shared_ptr<float[]> right_buf(new float[ses_len_smp], std::default_delete<float[]>());

    memset(left_buf.get(), 0, ses_len_smp * sizeof(float));
    memset(right_buf.get(), 0, ses_len_smp * sizeof(float));

    const std::set<id_t> solo_tracks = getTracksWithSolo(session_id);
    LOG_F(INFO, !solo_tracks.empty() ? "Some tracks marked solo, mixing only them" :

                                       "Mixing all available tracks");

    const std::set<id_t> tracks_to_mix = !solo_tracks.empty() ? solo_tracks : session->getTracks();

    LOG_F(INFO, "Tracks going to mix: %s", Utils::idSetToString(tracks_to_mix).c_str());

    for (const auto track_id: tracks_to_mix)
    {
        const Track *track = TracksManager::getTrack(track_id);

        if (!track)
        {
            LOG_F(ERROR, "Cannot get track (id: '%d)", track_id);
            return false;
        }

        if (track->getMute())
        {
            LOG_F(INFO, "Track (id: '%d') is muted, skipping", track_id);

            continue;
        }

        LOG_F(INFO, "Mixing track (id: %d)", track_id);

        const std::set<id_t> fragments = track->getFragments();

        LOG_F(INFO, "Track (id: '%d') has '%d' fragments", track->getId(), fragments.size());

        for (const id_t fragment_id: fragments)
        {
            LOG_F(INFO, "Mixing fragment (id: '%d')", fragment_id);

            const Fragment *fragment = FragmentsManager::getFragment(fragment_id);

            if (!fragment)
            {
                LOG_F(ERROR, "Failed to get fragment (id: %d)", fragment_id);
                return false;
            }

            LOG_F(INFO, "Fragment (id: %d) is linked with audio (id: %d)", fragment_id, fragment->getAudio());

            LOG_F(INFO, "Mixing left channel");
            if (!mixAudioToOutBuffer(fragment_id, left_buf.get(), ses_len_smp, track->getGain(), track->getLevel()))
            {
                return false;
            }

            LOG_F(INFO, "Mixing right channel");
            if (!mixAudioToOutBuffer(fragment_id, right_buf.get(), ses_len_smp, track->getGain(), track->getLevel()))
            {
                return false;
            }
        }
    }

    const std::string file_ext = Utils::getFileExt(mix_path);
    LOG_F(INFO, "Searching for codec with target file extension: %s", file_ext.c_str());
    const CodecManager::Codec* codec = CodecManager::findCodecByFileExt(file_ext);

    if (!codec)
    {
        LOG_F(ERROR, "Cannot find codec for file extension: '%s'", file_ext.c_str());
        return false;
    }

    LOG_F(INFO, "Got codec: %s", codec->getName());

    float* arr[2] = { left_buf.get(), right_buf.get() };
    
    LOG_F(INFO, "Saving file with path: '%s'; length: '%d' (smp); bytes per sample: '%d'; channels num: %d; sample rate: '%d'",
                mix_path.c_str(), ses_len_smp, 2, 1, session->getSampleRate());

    if (codec->saveFile(CodecFileInfo(mix_path, arr, ses_len_smp, 1, session->getSampleRate()), 2))
    {
        LOG_F(ERROR, "Cannot save file");
        return false;
    }

    LOG_F(INFO, "Session (id: '%d') was rendered", session_id);
    return true;
}