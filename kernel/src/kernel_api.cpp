#include "kernel_api.h"
#include "kernel.h"
#include "terminal.h"


#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <set>
#include <queue>
#include <map>

static callback_t g_cmd_transmitter;
static EKernelAPIStatus cmdReceiver(std::string cmd);

class CommandSeq
{
private:
	std::queue<std::string> tokens;

public:

	CommandSeq(std::string cmd_line)
	{
		std::stringstream ss(cmd_line);
		std::string t;

		while (std::getline(ss, t, ' '))
		{
			tokens.push(t);
		}
	}

	std::string sliceNextToken()
	{
		if (tokens.empty())
		{
			return std::string();
		}

		std::string t = tokens.front();
		tokens.pop();

		return t;
	}

	bool hasNTokens(uint8_t n)
	{
		return tokens.size() == n;
	}
};

template<typename ENUM>
class IdentsMap
{
private:
	const std::map<std::string, ENUM> m_idents;

public:
	IdentsMap(std::initializer_list<std::pair<const std::string, ENUM>> init_list)
		: m_idents(init_list)
	{
	}

	bool hasIdent(std::string token)
	{
		return m_idents.find(token) != m_idents.end();
	}

	ENUM getIdent(std::string token)
	{
		return m_idents.at(token);
	}
};

void initKernelAPI()
{
	g_cmd_transmitter = spawnTerminal(cmdReceiver);
}


static EKernelAPIStatus cmdInit(CommandSeq seq)
{
	enum EIdents { eCodecs, eEffects, eIOs, eNone };
	IdentsMap<EIdents> idents_map{ {"codecs", eCodecs }, {"effects", eEffects}, {"ios", eIOs} };

	std::string token = seq.sliceNextToken();

	EIdents cmd = idents_map.hasIdent(token) ?
				  idents_map.getIdent(token) : eNone;

	switch (cmd)
	{
	case eCodecs:
	{
		if (Kernel::initKernelCodecs() == 0)
		{
			g_cmd_transmitter("codecs inited");
			return EKernelAPIStatus::eOk;
		}
		g_cmd_transmitter("error");

		break;
	}
	case eEffects:
		break;

	case eIOs:
		break;
	}

	return EKernelAPIStatus::eErr;
}

static EKernelAPIStatus cmdList(CommandSeq seq)
{
	enum EIdents { eCodecs, eEffects, eIOs, eNone };
	IdentsMap<EIdents> idents_map{ {"codecs", eCodecs }, {"effects", eEffects}, {"ios", eIOs} };

	std::string token = seq.sliceNextToken();

	EIdents cmd = idents_map.hasIdent(token) ?
				  idents_map.getIdent(token) : eNone;

	switch (cmd)
	{
	case eCodecs:
	{
		g_cmd_transmitter(Kernel::listInitedCodecs());
		break;
	}
	case eEffects:
		break;

	case eIOs:
		break;
	}

	return EKernelAPIStatus::eErr;
}

static EKernelAPIStatus cmdSession(CommandSeq seq)
{
	enum EIdents { eLoad, eCreate, eSave, eNone };
	IdentsMap<EIdents> idents_map{ {"load", eLoad }, {"create", eCreate}, {"save", eSave} };

	std::string token = seq.sliceNextToken();

	EIdents cmd = idents_map.hasIdent(token) ?
				  idents_map.getIdent(token) : eNone;


	switch (cmd)
	{
	case eLoad:
	{
		g_session = genDummySession();
		g_cmd_transmitter(g_session ? "loaded" : "err");

		break;
	}
	case eCreate:
    {
        if (!seq.hasNTokens(3))
        {
            g_cmd_transmitter("invalid parameters");
            return EKernelAPIStatus::eErr;
        }

        std::string name = seq.sliceNextToken();
        std::string path = seq.sliceNextToken();
        std::string smp_rate_str = seq.sliceNextToken();

        uint32_t smp_rate = stoi(smp_rate_str);

        if (smp_rate != 48000)
        {
            g_cmd_transmitter("supporting only 48kHz format");
            return EKernelAPIStatus::eErr;
        }

        status_t status = Kernel::createSession(name, path, smp_rate);

        if (status != 0)
        {
            g_cmd_transmitter("Warn: command exited with code: " + std::to_string(status));
            return EKernelAPIStatus::eWarn;
        }

        return EKernelAPIStatus::eOk;
    }
		break;

	case eSave:
		break;
	}

	return EKernelAPIStatus::eErr;
}

static EKernelAPIStatus cmdRender(CommandSeq seq)
{
	enum EIdents {eNone, eAll};
	IdentsMap<EIdents> idents_map{{"all", eAll}, {"all", eAll}};
	std::string token = seq.sliceNextToken();

	EIdents cmd = idents_map.hasIdent(token) ? 
				  idents_map.getIdent(token) : eNone;
	
	switch (cmd)
	{
	case eAll:
	{
		if (!seq.hasNTokens(1))
		{
			g_cmd_transmitter("err: invalid syntax for render all");

			return EKernelAPIStatus::eOk;
		}

		std::string mix_name = seq.sliceNextToken();
		
		if (Kernel::renderAll(mix_name) != 0)
		{
			g_cmd_transmitter("rendered");
			return EKernelAPIStatus::eOk;
		}

		g_cmd_transmitter("err: rendered");
		
		break;
	}
	default:
		break;
	}

	return EKernelAPIStatus::eErr;
}

static EKernelAPIStatus cmdTrack(CommandSeq seq)
{
	enum EIdents { eAdd, eRemove, eMute, eSolo, eVolume, eGain, ePan, eEffect, eNone };
	IdentsMap<EIdents> idents_map{ {"add", eAdd}, {"remove", eRemove}, {"mute", eMute},
								   {"solo", eSolo}, {"volume", eVolume}, {"gain", eGain},
								   {"pan", ePan}, {"effect", eEffect} };

	std::string token = seq.sliceNextToken();

	EIdents cmd = idents_map.hasIdent(token) ?
				  idents_map.getIdent(token) : eNone;

	switch (cmd)
	{
	default:
		break;
	}

	return EKernelAPIStatus::eErr;
}

static EKernelAPIStatus cmdReceiver(std::string user_cmd_line)
{
	CommandSeq seq(user_cmd_line);

	enum EIdents { eInit, eList, eSession, eQuit, ePlayback, eRender, eNone };
	IdentsMap<EIdents> idents_map{ {"init", eInit }, {"list", eList}, {"session", eSession}, {"quit", eQuit},
								   {"playback", ePlayback}, {"render", eRender} };
	
	std::string token = seq.sliceNextToken();

	EIdents cmd = idents_map.hasIdent(token) ?
				  idents_map.getIdent(token) : eNone;	

	switch (cmd)
	{
	case eInit:
		return cmdInit(seq);

	case eList:
		return cmdList(seq);

	case eSession:
		return cmdSession(seq);

	case eRender:
		return cmdRender(seq);
	}


	return EKernelAPIStatus::eErr;
}