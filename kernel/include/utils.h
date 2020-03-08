#pragma once
#include <common.h>

namespace Utils
{
    std::string idSetToString(const std::set<id_t>& id_set);
    std::string getFileExt(const std::string& s);
}