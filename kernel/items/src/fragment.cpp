#include "fragment.h"
#include "audios_manager.h"
#include "tracks_manager.h"
#include "sessions_manager.h"

Fragment::Fragment(const id_t id, const id_t linked_track, const id_t linked_audio)
                 : m_id(id)
                 , m_linked_audio(linked_audio)
                 , m_linked_track(linked_track)
                 , m_time_offset(0)
                 , m_crop_from(0)
                 , m_crop_to(0)
{
    const Audio* audio = AudiosManager::getAudio(m_linked_audio);
    const Track* track = TracksManager::getTrack(m_linked_track);
    const Session* session = !track ? nullptr : SessionsManager::getSession(track->getSessionId());

    if (audio && session)
    {
        m_crop_to = session->samplesToMs(audio->getBufferLength());
    }
}

uint32_t Fragment::getTimeOffset() const { return m_time_offset; }
status_t Fragment::setTimeOffset(uint32_t offset)
{
    m_time_offset = offset;
    return 0;
}

uint32_t Fragment::getCropFrom() const { return m_crop_from; }
uint32_t Fragment::getCropTo() const { return m_crop_to; }

id_t Fragment::getAudio() const { return m_linked_audio; }
id_t Fragment::getTrack() const { return m_linked_track; }

status_t Fragment::crop(uint32_t crop_from, uint32_t crop_to)
{
    const Audio* audio = AudiosManager::getAudio(m_linked_audio);
    const Track* track = TracksManager::getTrack(m_linked_track);
    const Session* session = !track ? nullptr : SessionsManager::getSession(track->getSessionId());

    if (!audio || !session)
    {
        return 1;
    }

    const uint32_t audio_ms_len = session->samplesToMs(audio->getBufferLength());

    if (crop_from >= crop_to)
    {
        return 1;
    }

    if (crop_to > audio_ms_len)
    {
        return 1;
    }

    m_crop_from = crop_from;
    m_crop_to = crop_to;

    return 0;
}

id_t Fragment::getId() const { return m_id; }