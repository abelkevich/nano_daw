#include "api.h"
#include "api_audio.h"
#include "api_codec.h"
#include "api_effect.h"
#include "api_fragment.h"
#include "api_playback.h"
#include "api_render.h"
#include "api_session.h"
#include "api_track.h"

#include <windows.h>

static callback_t g_cmd_transmitter;

namespace ClientAPI
{

    static std::string fromErrCodeToString(EErrCodes err_code)
    {
        switch (err_code)
        {
        case EErrCodes::eInvalidFragment:
            return "invalid fragment id";
        case EErrCodes::eInvalidAudio:
            return "invalid audio id";
        case EErrCodes::eInvalidTrack:
            return "invalid track id";
        case EErrCodes::eInvalidArgsNum:
            return "invalid args number";
        case EErrCodes::eUnimplementedMethod:
            return "unimplemented method";
        case EErrCodes::eInvalidSession:
            return "invalid session";
        case EErrCodes::eOperationFailed:
            return "operation failed";
        case EErrCodes::eInvalidArgValue:
            return "invalid arg value";
        case EErrCodes::eCommandNotFound:
            return "cannot find command";
        }

        return "cannot parse err code";
    }

    json jsonErrResponse(EErrCodes err_code)
    {
        return json({ {"error", { {"code", err_code}, {"msg", fromErrCodeToString(err_code)}}} });
    }

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

	status_t initAPI(std::string client_path)
	{
        HINSTANCE hinstLib = LoadLibrary(TEXT(client_path.c_str()));

        if (!hinstLib)
        {
            return 1;
        }

        spawnClient_t spawn_client = (spawnClient_t) GetProcAddress(hinstLib, "spawnClient");

        if (!spawn_client)
        {
            return 2;
        }

        spawn_client(cmdReceiver);

        return 0;
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
			return cmdCodec(seq).dump();

		case eEffect:
			return cmdEffect(seq).dump();

		case eFragment:
			return cmdFragment(seq).dump();

		case eAudio:
			return cmdAudio(seq).dump();

		case eTrack:
			return cmdTrack(seq).dump();

		case eSession:
			return cmdSession(seq).dump();

		case ePlayback:
			return cmdPlayback(seq).dump();

		case eRender:
			return cmdRender(seq).dump();

		case eQuit:
			g_working = false;
            return json({"status", "ok"}).dump();
		}

        return jsonErrResponse(EErrCodes::eCommandNotFound).dump();
	}
}