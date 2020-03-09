#include "tracks_manager.h"
#include "fragments_manager.h"
#include "sessions_manager.h"

namespace TracksManager
{
    static std::map<id_t, Track*> g_tracks;

    id_t createTrack(const id_t session_id, const std::string& name)
    {
        LOG_F(INFO, "Creating track with session (id: '%d') and name: '%s'", session_id, name.c_str());

        Session* session = SessionsManager::getSession(session_id);

        if (!session)
        {
            LOG_F(ERROR, "Cannot get session (id: '%d')", session_id);
            return 0;
        }

        Track* track = new Track(ItemsManager::genUniqueId(), session_id, name);

        LOG_F(INFO, "Track (id: '%d') was created", track->getId());
        g_tracks.insert(std::make_pair(track->getId(), track));

        session->linkTrack(track->getId());

        return track->getId();
    }

    bool removeTrack(id_t id)
    {
        auto RemoveFragments = [](const Track* track) -> void
        {
            LOG_F(INFO, "Removing track (id: '%d') linked fragments", track->getId());

            std::set<id_t> fragments_id = track->getFragments();
            for (const id_t fragment_id : fragments_id)
            {
                if (!FragmentsManager::removeFragment(fragment_id))
                {
                    LOG_F(ERROR, "Cannot remove fragment (id: '%d'), skipping", fragment_id);
                }
            }
        };

        auto UnlinkFromSession = [](const Track* track) -> void
        {
            LOG_F(INFO, "Unlinking track (id: '%d') from session (id: '%d')", 
                        track->getId(), track->getSessionId());
            Session* session = SessionsManager::getSession(track->getSessionId());

            if (!session)
            {
                LOG_F(ERROR, "Cannot get session (id: '%d')", track->getSessionId());
                return;
            }

            session->unlinkTrack(track->getId());
        };

        LOG_F(INFO, "Removing track (id: '%d')", id);

        const Track* track = getTrack(id);

        if (!track)
        {
            LOG_F(ERROR, "Cannot get track (id: '%d')", id);
            return false;
        }

        RemoveFragments(track);
        UnlinkFromSession(track);

        delete g_tracks.at(id);
        g_tracks.erase(id);

        LOG_F(INFO, "Track (id: '%d') was removed", id);

        return true;
    }

    std::set<id_t> getTracks()
    {
        std::set<id_t> set;

        for (auto t : g_tracks)
        {
            set.insert(t.first);
        }

        return set;
    }

    Track* getTrack(id_t id)
    {
        if (g_tracks.find(id) == g_tracks.end())
        {
            return nullptr;
        }

        return g_tracks.at(id);
    }
}