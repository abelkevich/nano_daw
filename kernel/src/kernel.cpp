#include "kernel.h"
#include "codec_manager.h"
#include "render.h"
#include "api.h"

Session *g_session = nullptr;

int main(int argc, char **argv)
{
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file("kernel.log", loguru::Truncate, loguru::Verbosity_MAX);

    if (argc <= 1)
    {
        LOG_F(ERROR, "Cannot find client libraries in the cmd parameters list");
        return 1;
    }

    LOG_F(INFO, "Initing '%d' clients passed by cmd params", argc - 1);
    for (uint8_t cmd_param_index = 1; cmd_param_index < argc; cmd_param_index++)
    {
        if (!ClientAPI::addAPIHandler(argv[cmd_param_index]))
        {
            LOG_F(ERROR, "Cannot add API handler");
            return 2;
        }
    }

    ClientAPI::runAPIHandlers();
    
    LOG_F(INFO, "Starting wait thread");
    while (ClientAPI::getOnlineClients() > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  
    return 0;
}
