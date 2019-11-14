#include <iostream>

#include "common.h"
#include "client_api.h"

static callback_t g_transmitter;
static std::thread g_wait_cmd_thread;

static void waitCmd()
{
    while (1)
    {
        std::string cmd;

        std::cout << "nano_daw shell> ";
        std::getline(std::cin, cmd);

        std::string api_response = g_transmitter(cmd);
        std::cout << api_response << std::endl;
    }
}

void spawnClient(callback_t transmitter)
{
    g_transmitter = transmitter;
    g_wait_cmd_thread = std::thread(waitCmd);
}