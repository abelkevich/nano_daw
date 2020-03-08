#include "track.h"
#include "tracks_manager.h"
#include "fragments_manager.h"

Track::Track(const id_t id, const id_t session_id, const std::string &name)
           : m_id(id)
           , m_linked_session_id(session_id)
           , m_name(name)
           , m_mute(false)
           , m_solo(false)
           , m_pan(0)
           , m_gain(0)
           , m_level(0)
{
}

std::set<id_t> Track::getFragments() const
{
    return m_fragments;
}

std::set<id_t> Track::getEffects() const
{
    return m_effects;
}

std::string Track::getName() const { return m_name; }

status_t Track::setName(std::string name)
{
    if (name.empty())
    {
        return 1;
    }

    if (name.size() > 30)
    {
        return 2;
    }

    m_name = name;
    return 0;
}

bool Track::getMute() const { return m_mute; }

status_t Track::setMute(bool mute)
{ 
    m_mute = mute; 
    return 0; 
}

bool Track::getSolo() const { return m_solo; }

status_t Track::setSolo(bool solo)
{
    m_solo = solo;
    return 0;
}

int8_t Track::getPan() const { return m_pan; }

status_t Track::setPan(int8_t pan)
{
    if (pan < -100 || pan > 100)
    {
        return 1;
    }

    m_pan = pan;

    return 0;
}

uint8_t Track::getGain() const { return m_gain; }

status_t Track::setGain(uint8_t gain)
{
    if (gain > 100)
    {
        return 1;
    }

    m_gain = gain;

    return 0;
}

uint8_t Track::getLevel() const { return m_level; }

status_t Track::setLevel(uint8_t level)
{
    if (level > 100)
    {
        return 1;
    }

    m_level = level;

    return 0;
}

bool Track::linkFragment(const id_t fragment_id)
{
    LOG_F(INFO, "Linking track (id: '%d') with fragment (id: '%d')", this->getId(), fragment_id);

    if (!FragmentsManager::getFragment(fragment_id))
    {
        LOG_F(ERROR, "Cannot get fragment (id: '%d')", fragment_id);
        return false;
    }

    if (m_fragments.find(fragment_id) != m_fragments.end())
    {
        LOG_F(ERROR, "Fragment (id: '%d') already linked to the track (id: '%d')", 
                     fragment_id, this->getId());
        return false;
    }

    m_fragments.insert(fragment_id); 
    LOG_F(INFO, "Fragment (id: '%d') was unlinked from track (id: '%d')", fragment_id, this->getId());

    return true;
}

bool Track::unlinkFragment(const id_t fragment_id)
{
    LOG_F(INFO, "Unlinking fragment (id: '%d') from track (id: '%d')", fragment_id, this->getId());

    if (m_fragments.find(fragment_id) == m_fragments.end())
    {
        LOG_F(ERROR, "Cannot find fragment (id: '%d') in track (id: '%d') linked fragments", 
                     fragment_id, this->getId());
        return false;
    }

    m_fragments.erase(fragment_id); 
    LOG_F(INFO, "Fragment (id: '%d') was unlinked from track (id: '%d')", fragment_id, this->getId());

    return true;
}


id_t Track::getId() const { return m_id; }
id_t Track::getSessionId() const { return m_linked_session_id; }