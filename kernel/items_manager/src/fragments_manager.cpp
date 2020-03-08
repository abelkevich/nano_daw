#include "fragments_manager.h"
#include "audios_manager.h"
#include "tracks_manager.h"

namespace FragmentsManager
{
    static std::map<id_t, Fragment*> g_fragments;

    id_t createFragment(const id_t linked_track, const id_t linked_audio)
    {
        LOG_F(INFO, "Creating new fragment with track (id: '%d') and audio (id: '%d')", linked_track, linked_audio);

        Audio* audio = AudiosManager::getAudio(linked_audio);
        if (!audio)
        {
            LOG_F(ERROR, "Cannot find linked audio (id: '%d')", linked_audio);
            return 0;
        }

        Track* track = TracksManager::getTrack(linked_track);
        if (!track)
        {
            LOG_F(ERROR, "Cannot find linked track (id: '%d')", linked_track);
            return 0;
        }

        Fragment* fragment = new Fragment(ItemsManager::genUniqueId(), linked_track, linked_audio);
        LOG_F(INFO, "Fragment (id: '%d') was created", fragment->getId());

        g_fragments.insert(std::make_pair(fragment->getId(), fragment));

        track->linkFragment(fragment->getId());
        audio->linkFragment(fragment->getId());

        return fragment->getId();
    }

    Fragment* getFragment(id_t id)
    {
        if (g_fragments.find(id) == g_fragments.end())
        {
            return nullptr;
        }

        return g_fragments.at(id);
    }

    bool removeFragment(id_t id)
    {
        auto UnlinkFromAudio = [](const Fragment* fragment) -> void
        {
            LOG_F(INFO, "Unlinking fragment (id: '%d') from audio (id: '%d')", fragment->getId(), fragment->getAudio());
            
            Audio* linked_audio = AudiosManager::getAudio(fragment->getAudio());

            if (!linked_audio)
            {
                LOG_F(ERROR, "Cannot get linked audio (id: '%d')", fragment->getAudio());
                return;
            }

            linked_audio->unlinkFragment(fragment->getId());
        };

        auto UnlinkFromTrack = [](const Fragment* fragment) -> void
        {
            LOG_F(INFO, "Unlinking fragment (id: '%d') from track (id: '%d')", fragment->getId(), fragment->getTrack());
            
            Track* linked_track = TracksManager::getTrack(fragment->getTrack());
            if (!linked_track)
            {
                LOG_F(ERROR, "Cannot get linked track (id: '%d')", fragment->getTrack());
                return;
            }

            linked_track->unlinkFragment(fragment->getId());
        };

        LOG_F(INFO, "Removing fragment (id: '%d')", id);

        Fragment* fragment = getFragment(id);

        if (!fragment)
        {
            LOG_F(ERROR, "Cannot get fragment (id: '%d')", id);
            return false;
        }

        UnlinkFromAudio(fragment);
        UnlinkFromTrack(fragment);
        
        delete g_fragments.at(id);
        g_fragments.erase(id);

        LOG_F(INFO, "Fragment (id: '%d') was removed", id);

        return true;
    }

    std::set<id_t> getFragments()
    {
        std::set<id_t> set;

        for (auto f : g_fragments)
        {
            set.insert(f.first);
        }

        return set;
    }
}