#pragma once
#include "common.h"

enum class EKernelAPIStatus
{
    eOk, eErr, eWarn
};

std::string APIStatusToString(EKernelAPIStatus status);

struct APIResponse
{
	EKernelAPIStatus status;
	std::string data;

	APIResponse(EKernelAPIStatus _status, std::string _data);
	APIResponse(EKernelAPIStatus _status);
};

typedef std::function<std::string(std::string)> callback_t;