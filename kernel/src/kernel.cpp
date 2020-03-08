#include "kernel.h"
#include "codec_manager.h"
#include "render.h"
#include "api.h"

bool g_working = true;
Session *g_session = nullptr;

int main(int argc, char **argv)
{
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file("kernel.log", loguru::Truncate, loguru::Verbosity_MAX);

    if (argc != 2)
    {
        LOG_F(ERROR, "Cannot find client library in the cmd parameters list");
        return 1;
    }

    LOG_F(INFO, "Initializing client API");
    if (!ClientAPI::initAPI(argv[1]))
    {
        LOG_F(ERROR, "Cannot init api");
        return 2;
    }

    LOG_F(INFO, "Starting wait thread");
    while (g_working)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  
    return 0;
}
