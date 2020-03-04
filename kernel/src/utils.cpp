#include "utils.h"

namespace Utils
{
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