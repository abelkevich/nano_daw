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

namespace ClientAPI
{
    static std::string cmdReceiverWrapper(std::string cmd);
    static json cmdReceiver(std::string cmd);

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

        spawn_client(cmdReceiverWrapper);

        return 0;
	}

    std::string cmdReceiverWrapper(std::string cmd)
    {
        json response = cmdReceiver(cmd);

        if (response.find("error") != response.end())
        {
            return json({ {"status", 1}, {"error", response.at("error")}}).dump();
        }

        if (!response.is_null())
        {
            return json({ {"status", 0}, {"data", response} }).dump();
        }

        return json({ {"status", 0} }).dump();
    }

    json cmdReceiver(std::string user_cmd_line)
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
            return json();
		}

        return jsonErrResponse(EErrCodes::eCommandNotFound);
	}
}