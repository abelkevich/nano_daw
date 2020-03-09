#pragma once
#include "common.h"

typedef void(*receiveResponse_t) (const std::string&);
typedef std::string(*getCommand_t)();
typedef bool(*init_t)();
