#pragma once
#include "items_manager.h"
#include "fragment.h"

namespace FragmentsManager
{
    id_t createFragment(id_t linked_track, id_t linked_audio);
    Fragment* getFragment(id_t id);
    std::set<id_t> getFragments();
    bool removeFragment(id_t id);
}
