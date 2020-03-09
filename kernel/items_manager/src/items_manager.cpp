#include "items_manager.h"

namespace ItemsManager
{
    id_t genUniqueId()
    {
        static id_t n = 1;
        return n++;
    }
}