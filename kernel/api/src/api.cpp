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

namespace ClientAPI
{
	std::string c_err_invalid_id = "invalid id";
	std::string c_err_invalid_args_number = "invalid args number";
	std::string c_err_unimplemented_method = "unimplemented method";
	std::string c_err_invalid_session = "invalid session";
	std::string c_err_operation_failed = "operation failed";
	std::string c_err_invalid_arg_value = "invalid arg value";
	std::string c_err_cannot_find_command = "cannot find command";

    static std::string cmdReceiver(std::string cmd);

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
		spawnTerminal(cmdReceiver);
	}

    std::string cmdReceiver(std::string user_cmd_line)
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
			return cmdCodec(seq).data;

		case eEffect:
			return cmdEffect(seq).data;

		case eFragment:
			return cmdFragment(seq).data;

		case eAudio:
			return cmdAudio(seq).dump();

		case eTrack:
			return cmdTrack(seq).data;

		case eSession:
			return cmdSession(seq).data;

		case ePlayback:
			return cmdPlayback(seq).data;

		case eRender:
			return cmdRender(seq).data;

		case eQuit:
			g_working = false;
            return APIResponse(EKernelAPIStatus::eOk).data;
		}

		return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command").data;
	}
}