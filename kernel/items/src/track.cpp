#include "track.h"
#include "items_manager.h"

Track::Track(std::string name)
    : m_name(name)
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

status_t Track::linkFragment(id_t fragment_id)
{
    if (!ItemsManager::getFragment(fragment_id))
    {
        return 2;
    }

    // Track already have this fragment
    if (m_fragments.find(fragment_id) != m_fragments.end())
    {
        return 3;
    }

    m_fragments.insert(fragment_id);
    return 0;
}

status_t Track::unlinkFragment(id_t fragment_id)
{
    if (!ItemsManager::getFragment(fragment_id))
    {
        return 2;
    }

    // Track don't have this fragment
    if (m_fragments.find(fragment_id) == m_fragments.end())
    {
        return 3;
    }

    m_fragments.erase(fragment_id);
    return 0;
}