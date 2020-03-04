#include "api.h"
#include "api_audio.h"
#include "api_codec.h"
#include "api_effect.h"
#include "api_fragment.h"
#include "api_playback.h"
#include "api_render.h"
#include "api_session.h"
#include "api_track.h"
#ifdef __linux__
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include <thread>

namespace ClientAPI
{
    static std::thread g_client_thread;
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

    bool initAPI(const std::string &client_path)
    {
        LOG_F(INFO, "Got client lib path: %s", client_path.c_str());
#ifdef __linux__
        void* hinstLib = dlopen(client_path.c_str(), RTLD_LAZY);
#else
        HINSTANCE hinstLib = LoadLibrary(TEXT(client_path.c_str()));
#endif

        if (!hinstLib)
        {
            LOG_F(ERROR, "Cannot load library");
            return false;
        }

        LOG_F(INFO, "Library loaded");
#ifdef __linux__
        spawnClient_t spawn_client = (spawnClient_t) dlsym(hinstLib, "spawnClient");
#else
        spawnClient_t spawn_client = (spawnClient_t) GetProcAddress(hinstLib, "spawnClient");
#endif
        if (!spawn_client)
        {
            LOG_F(ERROR, "Cannot find \"spawnClient\" procedure lib");
            return false;
        }

        LOG_F(INFO, "Found \"spawnClient\", now registering client callbacks and creating thread");

        g_client_thread = std::thread(spawn_client, cmdReceiverWrapper);

        return true;
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

        enum EIdents { eCodec, eEffect, eAudio, eFragment, eTrack, eSession, ePlayback, eRender, eNone };
        IdentsMap<EIdents> idents_map{ {"codec", eCodec }, {"effect", eEffect}, {"audio", eAudio}, {"fragment", eFragment}, {"track", eTrack},
                                       {"session", eSession}, {"playback", ePlayback}, {"render", eRender} };

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
        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}