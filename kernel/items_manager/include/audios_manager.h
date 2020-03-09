#pragma once

#include "items_manager.h"
#include "audio.h"

namespace AudiosManager
{
    id_t createAudio(const std::string& path);
    Audio* getAudio(id_t id);
    std::set<id_t> getAudios();
    bool removeAudio(id_t id);
}