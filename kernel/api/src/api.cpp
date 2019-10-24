#include "api.h"
#include "api_audio.h"
#include "api_codec.h"
#include "api_effect.h"
#include "api_fragment.h"
#include "api_playback.h"
#include "api_render.h"
#include "api_session.h"
#include "api_track.h"

static callback_t g_cmd_transmitter;
static EKernelAPIStatus cmdReceiver(std::string cmd);

namespace ClientAPI
{
	CommandSeq::CommandSeq(std::string cmd_line)
	{
		std::stringstream ss(cmd_line);
		std::string t;

		while (std::getline(ss, t, ' '))
		{
			tokens.push(t);
		}
	}

	std::string CommandSeq::sliceNextToken()
	{
		if (tokens.empty())
		{
			return std::string();
		}

		std::string t = tokens.front();
		tokens.pop();

		return t;
	}

	bool CommandSeq::hasNTokens(uint8_t n)
	{
		return tokens.size() == n;
	}

	void initAPI()
	{
		g_cmd_transmitter = spawnTerminal(cmdReceiver);
	}

	void sendToClient(std::string msg)
	{
		if (!g_cmd_transmitter)
		{
			return;
		}

		g_cmd_transmitter(msg);
	}

	EKernelAPIStatus cmdReceiver(std::string user_cmd_line)
	{
		CommandSeq seq(user_cmd_line);

		enum EIdents { eCodec, eEffect, eAudio, eFragment, eTrack, eSession, eQuit, ePlayback, eRender, eNone };
		IdentsMap<EIdents> idents_map{ {"codec", eCodec }, {"effect", eEffect}, {"audio", eAudio}, {"fragment", eFragment}, {"track", eTrack},
									   {"session", eSession}, {"quit", eQuit}, {"playback", ePlayback}, {"render", eRender} };

		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;

		switch (cmd)
		{
		case eCodec:
			return cmdCodec(seq);

		case eEffect:
			return cmdEffect(seq);

		case eFragment:
			return cmdFragment(seq);

		case eAudio:
			return cmdAudio(seq);

		case eTrack:
			return cmdTrack(seq);

		case eSession:
			return cmdSession(seq);

		case ePlayback:
			return cmdPlayback(seq);

		case eRender:
			return cmdRender(seq);

		case eQuit:
			g_working = false;
			return EKernelAPIStatus::eOk;
		}


		return EKernelAPIStatus::eErr;
	}
}