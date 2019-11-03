#include "terminal.h"
#include <iostream>

static callback_t g_transmitter;
static std::thread g_wait_cmd_thread;

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
        std::getline(std::cin, cmd);

        g_transmitter(cmd);
    }
}

callback_t spawnTerminal(callback_t transmitter)
{
    g_transmitter = transmitter;
    g_wait_cmd_thread = std::thread(waitCmd);

    return cmdReceiver;
}