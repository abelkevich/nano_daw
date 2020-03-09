#include "utils.h"
#include <sstream>
#include <filesystem>

namespace Utils
{
    std::string idSetToString(const std::set<id_t> &id_set)
    {
        std::stringstream str_stream;

        for (id_t id : id_set)
        {
            str_stream << id << "; ";
        }

        return str_stream.str();
    }

    std::string getFileExt(const std::string& file_name)
    {
        const size_t i = file_name.rfind('.', file_name.length());

        if (i != std::string::npos)
        {
            return(file_name.substr(i + 1, file_name.length() - i));
        }

        return std::string();
    }

    std::list <std::string> searchFilesByExt(const std::string& search_path, const std::string& target_ext)
    {    
        if (!std::filesystem::exists(search_path))
        {
            return std::list<std::string>();
        }

        std::list <std::string> file_list;

        for (const auto fs_item : std::filesystem::recursive_directory_iterator(search_path)) 
        {
            if (!std::filesystem::is_regular_file(fs_item.status()))
            {
                continue;
            }

            const std::string file_name = fs_item.path().filename().u8string();
                
            if (file_name.compare(file_name.size() - target_ext.size(), target_ext.size(), target_ext))
            {
                continue;
            }

            file_list.push_back(fs_item.path().u8string());
        }

        return file_list;
    }
}