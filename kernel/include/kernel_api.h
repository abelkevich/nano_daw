#pragma once
#include "common.h"

enum class EKernelAPIStatus
{
    eOk, eErr, eWarn
};

typedef std::function<EKernelAPIStatus(std::string)> callback_t;

void initKernelAPI();