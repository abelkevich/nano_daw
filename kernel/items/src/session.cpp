#include "session.h"
#include "tracks_manager.h"

Session::Session(const id_t id, const std::string &name, const std::string &path, const freq_t sample_rate)
               : m_id(id)
               , m_name(name)
               , m_path(path)
               , m_sample_rate(sample_rate)
{
}

bool Session::linkTrack(const id_t track_id)
{
    LOG_F(INFO, "Linking session (id: '%d') with track (id: '%d')", this->getId(), track_id);

    if (!TracksManager::getTrack(track_id))
    {
        LOG_F(ERROR, "Cannot get track (id: '%d')", track_id);
        return false;
    }

    if (m_tracks.find(track_id) != m_tracks.end())
    {
        LOG_F(ERROR, "Track (id: '%d') already linked to the session (id: '%d')", 
                     track_id, this->getId());
        return false;
    }

    LOG_F(INFO, "Track (id: '%d') was linked with session (id: '%d')", track_id, this->getId());
    m_tracks.insert(track_id);
    return true;
}

bool Session::unlinkTrack(const id_t track_id)
{
    LOG_F(INFO, "Unlinking track (id: '%d') from session (id: '%d')", track_id, this->getId());

    if (m_tracks.find(track_id) == m_tracks.end())
    {
        LOG_F(ERROR, "Cannot find track (id: '%d') in session (id: '%d') linked tracks", 
                     track_id, this->getId());
        return false;
    }

    m_tracks.erase(track_id);
    LOG_F(INFO, "Track (id: '%d') was unlinked from session (id: '%d')", track_id, this->getId());

    return true;
}

freq_t Session::getSampleRate() const { return m_sample_rate; }

std::string Session::getName() const { return m_name; }

bool Session::setName(const std::string &name)
{
    if (name.empty())
    {
        LOG_F(ERROR, "Cannot rename session (id: '%d'): value is empty", m_id);
        return false;
    }

    if (name.size() > 30)
    {
        LOG_F(ERROR, "Cannot rename session (id: '%d'): value length > 30", m_id);
        return false;
    }

    m_name = name;

    return true;
}

std::string Session::getPath() const { return m_path; }

bool Session::setPath(const std::string &path)
{
    m_path = path;
    return true;
}

std::set<id_t> Session::getTracks() const
{
    return m_tracks;
}

smpn_t Session::msToSamples(ms_t ms) const
{
    return m_sample_rate / 1000 * ms;
}

ms_t Session::samplesToMs(smpn_t samples) const
{
    return (ms_t) (( (float) samples / m_sample_rate) * 1000);
}

id_t Session::getId() const { return m_id; }