#pragma once
#include "items_manager.h"
#include "session.h"

namespace SessionsManager
{
    id_t createSession(const std::string& name, const std::string& file_name, freq_t sample_rate);
    Session* getSession(id_t id);
    std::set<id_t> getSessions();
    bool removeSession(id_t id);
}