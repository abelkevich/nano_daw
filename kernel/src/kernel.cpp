#include "kernel.h"
#include "kernel_api.h"
#include "codec_manager.h"
#include "render.h"
#include "api.h"

bool g_working = true;
Session *g_session = nullptr;

int main(int argc, char **argv)
{
	ClientAPI::initAPI();

    while (g_working)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  
    return 0;
}
