#pragma once
#include "common.h"

typedef std::function<status_t(std::string)> callback_t;

void initKernelAPI();