#include "kernel_api.h"
#include "kernel.h"
#include "terminal.h"

#include "codec_manager.h"
#include "render.h"

#include <sstream>

static callback_t g_cmd_transmitter;

static status_t cmdReceiver(std::string cmd);

void initKernelAPI()
{
	g_cmd_transmitter = spawnTerminal(cmdReceiver);
}

static status_t cmdReceiver(std::string cmd)
{
	if (cmd == "init io")
	{
		// initIO();
		g_cmd_transmitter("io inited");
	}
	else if (cmd == "init effects")
	{
		// initEffects();
		g_cmd_transmitter("effects inited");
	}
	else if (cmd == "init codecs")
	{
		status_t status = initCodecs();
		if (!status)
		{
			g_cmd_transmitter("codecs inited");
		}
		else
		{
			g_cmd_transmitter("something get wrong! Status: " + std::to_string(status));
		}

	}
	else if (cmd == "get codecs")
	{
		std::vector<CodecInfo> codecs = getInitedCodecs();

		std::stringstream str_stream;
		str_stream << "Codecs available:\n";

		for (auto codec : codecs)
		{
			str_stream << "\t" << codec.lib_name << std::endl;
		}

		g_cmd_transmitter(str_stream.str());

	}
	else if (cmd == "load dummy session")
	{
		g_session = genDummySession();
		g_cmd_transmitter("loaded");
	}
	else if (cmd == "render")
	{
		if (g_session.state == ESessionState::eNonInited)
		{
			g_cmd_transmitter("no session loaded");
			return 1;
		}

		render(g_session, "res\\sample_mix.wav");
		g_cmd_transmitter("rendered");
	}
	else if (cmd == "quit")
	{
		g_working = false;
	}

	return 0;
}