#pragma once

#include <cinttypes>
#include <string>
#include <functional>
#include <algorithm>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <list>
#include "loguru.hpp"

#ifdef _WIN32
    # ifdef WIN_EXPORT
        #define EXPORTED __declspec(dllexport)
    # else
        #define EXPORTED __declspec(dllimport)
    # endif
#else
    #define EXPORTED
#endif

typedef uint8_t status_t;
typedef uint32_t id_t;
typedef uint32_t freq_t;
typedef uint32_t smpn_t;
typedef uint32_t ms_t;