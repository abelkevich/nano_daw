#include "fragment.h"
#include "audios_manager.h"
#include "tracks_manager.h"
#include "sessions_manager.h"
#include "resampler.h"

Fragment::Fragment(const id_t id, const id_t linked_track, const id_t linked_audio)
                 : m_id(id)
                 , m_linked_audio(linked_audio)
                 , m_linked_track(linked_track)
                 , m_time_offset(0)
                 , m_crop_from(0)
                 , m_crop_to(0)
{
    crop(0, getAudioLength());
}

ms_t Fragment::getTimelineOffset() const { return m_time_offset; }
bool Fragment::setTimelineOffset(ms_t offset)
{
    m_time_offset = offset;
    return true;
}

uint32_t Fragment::getCropFrom() const { return m_crop_from; }
uint32_t Fragment::getCropTo() const { return m_crop_to; }

id_t Fragment::getAudio() const { return m_linked_audio; }
id_t Fragment::getTrack() const { return m_linked_track; }

ms_t Fragment::getAudioLength() const
{
    Track* track = TracksManager::getTrack(m_linked_track);
    
    if (!track)
    {
        return 0;
    }

    Session* session = SessionsManager::getSession(track->getSessionId());

    if (!session)
    {
        return 0;
    }

    Audio* audio = AudiosManager::getAudio(m_linked_audio);

    if (!audio)
    {
        return 0;
    }

    if (session->getSampleRate() != audio->getSampleRate())
    {
        float factor = Resampler::resampleFactor(audio->getSampleRate(), session->getSampleRate());
        return session->samplesToMs((smpn_t)(audio->getBufferLength() * factor));
    }
    
    return session->samplesToMs(audio->getBufferLength());
}

bool Fragment::crop(ms_t crop_from, ms_t crop_to)
{
    if (crop_from >= crop_to)
    {
        LOG_F(ERROR, "Cannot crop fragment (id: '%d'): crop_from ('%d') >= crop_to ('%d')"
                   , m_id, crop_from, crop_to);
        return false;
    }

    if (crop_to > getAudioLength())
    {
        LOG_F(ERROR, "Cannot crop fragment (id: '%d'): crop_to ('%d') > audio length ('%d')"
                   , m_id, crop_from, getAudioLength());
        return false;
    }

    m_crop_from = crop_from;
    m_crop_to = crop_to;

    return true;
}

id_t Fragment::getId() const { return m_id; }