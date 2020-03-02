#include "kernel.h"
#include "codec_manager.h"
#include "render.h"
#include "api.h"

bool g_working = true;
Session *g_session = nullptr;

int main(int argc, char **argv)
{
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file("nano_daw.log", loguru::Truncate, loguru::Verbosity_MAX);

    if (argc != 2)
    {
        LOG_F(ERROR, "Cannot find client in cmd line args");
        return 1;
    }

    LOG_F(INFO, "Starting initializing client API with client %s", argv[1]);
    if (ClientAPI::initAPI(argv[1]) != 0)
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
