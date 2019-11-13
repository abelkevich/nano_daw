#pragma once
#include "common.h"

typedef std::string(*callback_t) (std::string);
typedef void(*spawnClient_t)(callback_t);

#ifdef __cplusplus 
extern "C" {
#endif

    EXPORTED void __cdecl spawnClient(callback_t transmitter);

#ifdef __cplusplus
}
#endif