#pragma once
#include <common.h>

namespace Utils
{
    std::string idSetToString(const std::set<id_t>& id_set);
    std::string getFileExt(const std::string& file_name);
    std::list <std::string> searchFilesByExt(const std::string& search_path, const std::string& target_ext);
}