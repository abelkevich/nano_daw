#include "session.h"
#include "items_manager.h"

Session::Session(std::string name, std::string path, uint32_t sample_rate)
    : m_name(name)
    , m_path(path)
    , m_sample_rate(sample_rate)
{
}

status_t Session::linkTrack(id_t track_id)
{
    if (!ItemsManager::getTrack(track_id))
    {
        return 1;
    }

    m_tracks.insert(track_id);
    return 0;
}

uint32_t Session::getSampleRate() const { return m_sample_rate; }

std::string Session::getName() const { return m_name; }

status_t Session::setName(std::string name)
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

std::string Session::getPath() const { return m_path; }

status_t Session::setPath(std::string path)
{
    m_path = path;
    return 0;
}

std::set<id_t> Session::getTracks() const
{
    return m_tracks;
}

uint32_t Session::msToSamples(uint32_t ms)
{
    return m_sample_rate / 1000 * ms;
}

uint32_t Session::samplesToMs(uint32_t samples)
{
    return (float)samples / m_sample_rate * 1000;
}