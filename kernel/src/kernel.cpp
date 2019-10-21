#include "kernel.h"
#include "kernel_api.h"

bool g_working = true;
Session g_session;

int main(int argc, char **argv)
{
	initKernelAPI();

    while (g_working)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  
    return 0;
}
