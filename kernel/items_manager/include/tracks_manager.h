#pragma once
#include "items_manager.h"
#include "track.h"

namespace TracksManager
{
    id_t createTrack(id_t session_id, const std::string& name);
    Track* getTrack(id_t id);
    std::set<id_t> getTracks();
    bool removeTrack(id_t id);
}