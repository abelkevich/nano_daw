#pragma once
#include <common.h>

namespace Utils
{
    #ifdef __linux__
        const std::string c_lib_extension = ".so";
    #else
        const std::string c_lib_extension = ".dll";
    #endif

    void* loadLibrary(const std::string &file_path);
    void* getLibProcedure(void* lib_instance, const std::string &proc_name);
    std::string idSetToString(const std::set<id_t>& id_set);
    std::string getFileExt(const std::string& file_name);
    std::list <std::string> searchFilesByExt(const std::string& search_path, const std::string& target_ext);
}