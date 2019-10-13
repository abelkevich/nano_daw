#pragma once

#include <cinttypes>
#include <string>
#include <functional>
#include <thread>
#include <vector>

typedef uint8_t status_t;
typedef std::function<status_t(std::string)> callback_t;
