#include "terminal.h"
#include <iostream>

static callback_t g_transmitter;
static std::thread g_wait_cmd_thread;

std::string APIStatusToString(EKernelAPIStatus status)
{
	switch (status)
	{
	case EKernelAPIStatus::eOk:
		return "Ok";
	case EKernelAPIStatus::eErr:
		return "Error";
	case EKernelAPIStatus::eWarn:
		return "Warning";
	}

	return "err-parsing-status";
}

static EKernelAPIStatus cmdReceiver(std::string cmd)
{
    std::cout << "Received: \n";
    std::cout << cmd << std::endl;

    return EKernelAPIStatus::eOk;
}

void waitCmd()
{
    while (1)
    {
        std::string cmd;

		std::cout << "nano_daw shell> ";
        std::getline(std::cin, cmd);

		APIResponse api_response = g_transmitter(cmd);

		if (api_response.status == EKernelAPIStatus::eOk && api_response.data.empty())
		{
			continue;
		}

		std::cout << '[' << APIStatusToString(api_response.status) << "]: ";
		std::cout << api_response.data << std::endl;
    }
}

void spawnTerminal(callback_t transmitter)
{
    g_transmitter = transmitter;
    g_wait_cmd_thread = std::thread(waitCmd);
}