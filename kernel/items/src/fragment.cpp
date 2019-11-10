#include "fragment.h"
#include "items_manager.h"

Fragment::Fragment(id_t linked_audio)
    : m_linked_audio(linked_audio)
    , m_time_offset(0)
    , m_crop_from(0)
    , m_crop_to(0)
{
    Audio* audio = ItemsManager::getAudio(m_linked_audio);
    if (audio && g_session)
    {
        m_crop_to = g_session->samplesToMs(audio->getBufferLength());
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

status_t Fragment::crop(uint32_t crop_from, uint32_t crop_to)
{
    if (!g_session)
    {
        return 1;
    }

    Audio* audio = ItemsManager::getAudio(m_linked_audio);
    if (!audio)
    {
        return 2;
    }

    uint32_t audio_ms_len = g_session->samplesToMs(audio->getBufferLength());

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