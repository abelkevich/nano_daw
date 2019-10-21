#pragma once

#include <cinttypes>
#include <string>
#include <functional>
#include <thread>
#include <vector>

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