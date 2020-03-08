#include "sessions_manager.h"
#include "tracks_manager.h"

namespace SessionsManager
{
    static std::map<id_t, Session*> g_sessions;

    id_t createSession(const std::string& name, const std::string& file_name, const freq_t sample_rate)
    {
        LOG_F(INFO, "Creating new session with name: '%s', file name: '%s', sample rate: '%d'", 
                    name.c_str(), file_name.c_str(), sample_rate);
        Session* session = new Session(ItemsManager::genUniqueId(), name, file_name, sample_rate);

        LOG_F(INFO, "Session (id: '%d') was created", session->getId());

        g_sessions.insert(std::make_pair(session->getId(), session));
        return session->getId();
    }

    Session* getSession(id_t id)
    {
        if (g_sessions.find(id) == g_sessions.end())
        {
            return nullptr;
        }

        return g_sessions.at(id);
    }

    bool removeSession(id_t id)
    {
        auto RemoveTracks = [](Session* session) -> void
        {
            LOG_F(INFO, "Removing session (id: '%d') linked tracks", session->getId());
            std::set<id_t> linked_tracks_id = session->getTracks();
            for (id_t linked_track_id : linked_tracks_id)
            {
                if (!TracksManager::removeTrack(linked_track_id))
                {
                    LOG_F(ERROR, "Cannot remove track (id: '%d'), skipping", linked_track_id);
                    continue;
                }
            }
        };

        LOG_F(INFO, "Removing session (id: '%d')", id);

        Session* session = getSession(id);

        if (!session)
        {
            LOG_F(ERROR, "Cannot get session (id: '%d')", id);
            return false;
        }

        RemoveTracks(session);

        delete g_sessions.at(id);
        g_sessions.erase(id);

        LOG_F(INFO, "Session (id: '%d') was removed", id);

        return true;
    }

    std::set<id_t> getSessions()
    {
        std::set<id_t> set;

        for (auto t : g_sessions)
        {
            set.insert(t.first);
        }

        return set;
    }
}