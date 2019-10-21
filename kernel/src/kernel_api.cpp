#include "kernel_api.h"
#include "kernel.h"
#include "terminal.h"

#include "codec_manager.h"
#include "render.h"

#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <set>
#include <queue>
#include <map>

static callback_t g_cmd_transmitter;
static status_t cmdReceiver(std::string cmd);
static std::queue<std::string> splitString(const std::string& str, char delim);

void initKernelAPI()
{
	g_cmd_transmitter = spawnTerminal(cmdReceiver);
}

std::queue<std::string> splitString(const std::string& str, char delim)
{
	std::queue<std::string> tokens;
	std::stringstream ss(str);
	std::string t;
	
	while (std::getline(ss, t, delim)) 
	{
		tokens.push(t);
	}

	return tokens;
}

void cmdInit(std::queue<std::string> tokens)
{
	enum EIdents { eCodecs, eEffects, eIOs};
	static const std::map<std::string, EIdents> idents_map = { {"codecs", eCodecs }, {"effects", eEffects}, {"ios", eIOs} };

	auto id_it = idents_map.find(tokens.front());

	if (id_it == idents_map.end())
	{
		// log err
		return;
	}

	tokens.pop();

	switch (id_it->second)
	{
	case eCodecs:
	{
		if (initCodecs() == 0)
		{
			g_cmd_transmitter("codecs inited");
			return;
		}
		g_cmd_transmitter("error");

		break;
	}
	case eEffects:
		break;

	case eIOs:
		break;

	default:
		break;
	}
}

void cmdList(std::queue<std::string> tokens)
{
	enum EIdents { eCodecs, eEffects, eIOs };
	static const std::map<std::string, EIdents> idents_map = { {"codecs", eCodecs }, {"effects", eEffects}, {"ios", eIOs} };

	auto id_it = idents_map.find(tokens.front());

	if (id_it == idents_map.end())
	{
		// log err
		return;
	}

	tokens.pop();

	switch (id_it->second)
	{
	case eCodecs:
	{
		std::vector<CodecInfo> codecs = getInitedCodecs();

		std::stringstream str_stream;
		str_stream << "Codecs available:\n";

		for (auto codec : codecs)
		{
			str_stream << "\t" << codec.lib_name << std::endl;
		}

		g_cmd_transmitter(str_stream.str());

		break;
	}
	case eEffects:
		break;

	case eIOs:
		break;

	default:
		break;
	}
}

void cmdSession(std::queue<std::string> tokens)
{
	enum EIdents { eLoad, eCreate, eSave };
	static const std::map<std::string, EIdents> idents_map = { {"load", eLoad }, {"create", eCreate}, {"save", eSave} };

	auto id_it = idents_map.find(tokens.front());

	if (id_it == idents_map.end())
	{
		// log err
		return;
	}

	tokens.pop();

	switch (id_it->second)
	{
	case eLoad:
	{
		g_session = genDummySession();

		break;
	}
	case eCreate:
		break;

	case eSave:
		break;

	default:
		break;
	}
}

void cmdRender(std::queue<std::string> tokens)
{
	enum EIdents { eAll};
	static const std::map<std::string, EIdents> idents_map = { {"all", eAll} };

	auto id_it = idents_map.find(tokens.front());

	if (id_it == idents_map.end())
	{
		// log err
		return;
	}

	tokens.pop();

	switch (id_it->second)
	{
	case eAll:
	{
		if (g_session.state == ESessionState::eNonInited)
		{
			g_cmd_transmitter("no session loaded");
			return;
		}

		render(g_session, "res\\sample_mix.wav");
		g_cmd_transmitter("rendered");
		break;
	}
	default:
		break;
	}
}

void cmdTrack(std::queue<std::string> tokens)
{
	enum EIdents { eAdd, eRemove, eMute, eSolo, eVolume, eGain, ePan, eEffect };
	static const std::map<std::string, EIdents> idents_map = { {"add", eAdd}, {"remove", eRemove}, {"mute", eMute},
															   {"solo", eSolo}, {"volume", eVolume}, {"gain", eGain},
															   {"pan", ePan}, {"effect", eEffect}};

	auto id_it = idents_map.find(tokens.front());

	if (id_it == idents_map.end())
	{
		// log err
		return;
	}

	tokens.pop();

	switch (id_it->second)
	{
	default:
		break;
	}
}

static status_t cmdReceiver(std::string cmd)
{
	std::queue<std::string> tokens = splitString(cmd, ' ');

	if (tokens.empty())
	{
		return 1;
	}

	enum EIdents { eInit, eList, eSession, eQuit, ePlayback, eRender };
	static const std::map<std::string, EIdents> idents_map = { {"init", eInit }, {"list", eList}, {"session", eSession}, {"quit", eQuit},
															   {"playback", ePlayback}, {"render", eRender} };
	
	auto id_it = idents_map.find(tokens.front());

	if (id_it == idents_map.end())
	{
		// log err
		return 1;
	}

	tokens.pop();

	switch (id_it->second)
	{
	case eInit:
		cmdInit(tokens);
		break;

	case eList:
		cmdList(tokens);
		break;

	case eSession:
		cmdSession(tokens);
		break;

	case eRender:
		cmdRender(tokens);
		break;

	default:
		return 1;
		break;
	}


	return 0;
}