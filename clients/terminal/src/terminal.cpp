#include <iostream>

#include "common.h"
#include "client_api.h"

extern "C" EXPORTED std::string getCommand()
{
    std::string cmd;

    std::cout << "nano_daw shell> ";
    std::getline(std::cin, cmd);

    return cmd;
}

extern "C" EXPORTED void receiveResponse(const std::string& response)
{
    std::cout << response << "\n";
}

extern "C" EXPORTED bool init()
{
    return true;
}