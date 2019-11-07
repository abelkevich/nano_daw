#include "kernel_api.h"

APIResponse::APIResponse(EKernelAPIStatus _status)
					   : status(_status)
{
}

APIResponse::APIResponse(EKernelAPIStatus _status, std::string _data)
					   : status(_status), data(_data)
{
}