#include "utils.h"
#include <sstream>
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

    std::string getFileExt(const std::string& s)
    {
        const size_t i = s.rfind('.', s.length());

        if (i != std::string::npos)
        {
            return(s.substr(i + 1, s.length() - i));
        }

        return std::string();
    }
}