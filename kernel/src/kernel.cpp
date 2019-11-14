#include "kernel.h"
#include "codec_manager.h"
#include "render.h"
#include "api.h"

bool g_working = true;
Session *g_session = nullptr;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        return 1;
    }

    if (ClientAPI::initAPI(argv[1]) != 0)
    {
        return 2;
    }

    while (g_working)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  
    return 0;
}
