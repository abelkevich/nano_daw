#include <iostream>

#include "common.h"
#include "client_api.h"

void spawnClient(callback_t transmitter)
{
    std::string cmd;

    do
    {
        std::cout << "nano_daw shell> ";
        std::getline(std::cin, cmd);

        std::string api_response = transmitter(cmd);
        std::cout << api_response << std::endl;
    } while (cmd != "quit");
}