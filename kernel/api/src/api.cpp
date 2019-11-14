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
    const std::string c_err_invalid_fragment_id_str = "invalid fragment id";
    const uint8_t c_err_invalid_fragment_id_code = 1;

    const std::string c_err_invalid_audio_id_str = "invalid audio id";
    const uint8_t c_err_invalid_audio_id_code = 2;

    const std::string c_err_invalid_track_id_str = "invalid track id";
    const uint8_t c_err_invalid_track_id_code = 3;

    const std::string c_err_invalid_args_number_str = "invalid args number";
    const uint8_t c_err_invalid_args_number_code = 4;

    const std::string c_err_unimplemented_method_str = "unimplemented method";
    const uint8_t c_err_unimplemented_method_code = 5;

    const std::string c_err_invalid_session_str = "invalid session";
    const uint8_t c_err_invalid_session_code = 6;

    const std::string c_err_operation_failed_str = "operation failed";
    const uint8_t c_err_operation_failed_code = 7;

    const std::string c_err_invalid_arg_value_str = "invalid arg value";
    const uint8_t c_err_invalid_arg_value_code = 8;

    const std::string c_err_cannot_find_command_str = "cannot find command";
    const uint8_t c_err_cannot_find_command_code = 9;

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

        return json({ {"error", { {"code", c_err_cannot_find_command_code}, {"msg", c_err_cannot_find_command_str}}} }).dump();
	}
}