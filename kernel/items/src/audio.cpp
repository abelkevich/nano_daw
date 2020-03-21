#include "audio.h"
#include "fragments_manager.h"

Audio::Audio(const id_t id, const std::string &path, float* buffer, const uint32_t buffer_size, const freq_t sample_rate)
           : m_id(id)
           , m_path(path)
           , m_buffer(buffer)
           , m_buffer_size(buffer_size)
           , m_sample_rate(sample_rate)
{
}


std::string Audio::getPath() const { return m_path; }
uint32_t Audio::getBufferLength() const { return m_buffer_size; }
float* Audio::getBuffer() const { return m_buffer; }
freq_t Audio::getSampleRate() const { return m_sample_rate; }

bool Audio::linkFragment(const id_t fragment_id)
{
    LOG_F(INFO, "Linking audio (id: '%d') to fragment (id: '%d')", this->getId(), fragment_id);

    if (!FragmentsManager::getFragment(fragment_id))
    {
        LOG_F(ERROR, "Cannot get fragment (id: '%d')", fragment_id);
        return false;
    }

    if (m_linked_fragments.find(fragment_id) != m_linked_fragments.end())
    {
        LOG_F(ERROR, "Fragment (id: '%d') already linked with audio (id: '%d')", 
                     fragment_id, this->getId());

        return false;
    }

    LOG_F(INFO, "Audio (id: '%d') was linked with fragment (id: '%d')", this->getId(), fragment_id);
    m_linked_fragments.insert(fragment_id);
    return true;
}

bool Audio::unlinkFragment(const id_t fragment_id)
{
    LOG_F(INFO, "Unlinking fragment (id: '%d') from audio (id: '%d')", fragment_id, this->getId());

    if (m_linked_fragments.find(fragment_id) == m_linked_fragments.end())
    {
        LOG_F(ERROR, "Cannot find fragment (id: '%d') in audio (id: '%d') linked fragments", 
                     fragment_id, this->getId());

        return false;
    }

    LOG_F(INFO, "Fragment (id: '%d') was unlinked from audio (id: '%d')", fragment_id, this->getId());
    m_linked_fragments.erase(fragment_id);
    return true;
}

id_t Audio::getId() const { return m_id; }
std::set<id_t> Audio::getLinkedFragments() const { return m_linked_fragments; }