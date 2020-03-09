#include "api.h"
#include "api_audio.h"
#include "api_codec.h"
#include "api_fragment.h"
#include "api_playback.h"
#include "api_render.h"
#include "api_session.h"
#include "api_track.h"

#include "utils.h"

#include <thread>
#include <mutex>

namespace ClientAPI
{
    struct ClientLib
    {
        ClientLib(getCommand_t _get_command_proc, receiveResponse_t _receive_response_proc, 
                  std::string _lib_file_name, void* _system_descriptor)
                : get_command_proc(_get_command_proc)
                , receive_response_proc(_receive_response_proc)
                , lib_file_name(_lib_file_name)
                , system_descriptor(_system_descriptor)
                , processing_thread(nullptr)
        {
        }

        getCommand_t get_command_proc;
        receiveResponse_t receive_response_proc;
        std::string lib_file_name;
        void* system_descriptor;
        std::thread* processing_thread;
    };

    // instance:ClientLib pair
    static std::map<void*, ClientLib> g_client_libs_map;
    static std::mutex s_commands_queue_mutex;

    static std::string cmdReceiverWrapper(std::string cmd);
    static json cmdReceiver(std::string cmd);
    static bool removeAPIHandler(void* instance);

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

    void processClient(void* system_descriptor)
    {
        LOG_F(INFO, "Starting processing client lib (instance: '%p')", system_descriptor);

        auto find_it = g_client_libs_map.find(system_descriptor);

        if (find_it == g_client_libs_map.end())
        {
            LOG_F(ERROR, "Cannot find lib info for instance: '%p'", system_descriptor);
            return;
        }

        const ClientLib &client_lib = find_it->second;

        while (true)
        {
            const std::string cmd = client_lib.get_command_proc();
            LOG_F(INFO, "Got request: '%s' from client (lib: '%s'; instance: '%p')", 
                        cmd.c_str(), client_lib.lib_file_name.c_str(), client_lib.system_descriptor);
            
            if (cmd == "quit")
            {
                LOG_F(INFO, "Removing API handler for instance '%p' and exiting from assiciated thread", system_descriptor);
                removeAPIHandler(system_descriptor);
                break;
            }

            s_commands_queue_mutex.lock();
            const std::string response = cmdReceiverWrapper(cmd);
            s_commands_queue_mutex.unlock();

            client_lib.receive_response_proc(response);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    void runAPIHandlers()
    {
        LOG_F(INFO, "Starting loaded API handlers");

        for (auto client : g_client_libs_map)
        {
            client.second.processing_thread = new std::thread(processClient, (void*) client.first);
        }
    }

    uint32_t getOnlineClients()
    {
        return g_client_libs_map.size();
    }

    bool removeAPIHandler(void* instance)
    {
        if (g_client_libs_map.find(instance) == g_client_libs_map.end())
        {
            LOG_F(ERROR, "Cannot find instance '%p' in api handlers map", instance);
            return false;
        }

        {
            const ClientLib& client_lib = g_client_libs_map.at(instance);
            LOG_F(INFO, "Removed API handler lib: '%s' instance: '%p'", client_lib.lib_file_name.c_str(), client_lib.system_descriptor);
        }

        g_client_libs_map.erase(instance);
        return true;
    }

    bool addAPIHandler(const std::string &client_path)
    {
        LOG_F(INFO, "Adding new API handler: '%s'", client_path.c_str());

        void* instance = Utils::loadLibrary(client_path);

        if (!instance)
        {
            LOG_F(ERROR, "Cannot load library: '%s'", client_path.c_str());
            return false;
        }

        if (g_client_libs_map.find((void*) instance) != g_client_libs_map.end())
        {
            LOG_F(ERROR, "Client (lib: '%s') already registered", client_path.c_str());
            return false;
        }

        receiveResponse_t receive_response_proc = (receiveResponse_t) Utils::getLibProcedure(instance, "receiveResponse");
        
        if (!receive_response_proc)
        {
            LOG_F(ERROR, "Cannot find 'receiveResponse' procedure");
            return false;
        }
        
        getCommand_t get_command_proc = (getCommand_t) Utils::getLibProcedure(instance, "getCommand");
        
        if (!get_command_proc)
        {
            LOG_F(ERROR, "Cannot find 'getCommand' procedure");
            return false;
        }
        
        init_t init_proc = (init_t) Utils::getLibProcedure(instance, "init");

        if (!init_proc)
        {
            LOG_F(ERROR, "Cannot find 'init' procedure");
            return false;
        }

        if (!init_proc())
        {
            LOG_F(ERROR, "Cannot init client (lib: '%s' instance: '%p')", client_path.c_str(), instance);
            return false;
        }

        ClientLib client_lib(get_command_proc, receive_response_proc, client_path, instance);
        g_client_libs_map.insert(std::make_pair(instance, client_lib));

        LOG_F(INFO, "API handler (lib: '%s' instance: '%p') successfully added", client_path.c_str(), instance);

        return true;
    }
    
    std::string cmdReceiverWrapper(std::string cmd)
    {
        json response = cmdReceiver(cmd);

        if (response.find("error") != response.end())
        {
            const std::string response_str = json({ {"status", 1}, {"error", response.at("error")} }).dump();
            LOG_F(ERROR, "Responding with error: '%s'", response_str.c_str());
            return response_str;
        }

        if (!response.is_null())
        {
            const std::string response_str = json({ {"status", 0}, {"data", response} }).dump();
            LOG_F(INFO, "Responding with data: '%s'", response_str.c_str());
            return response_str;
        }

        const std::string response_str = json({ {"status", 0} }).dump();
        LOG_F(INFO, "Responding without data: '%s'", response_str.c_str());
        return response_str;
    }

    json cmdReceiver(std::string user_cmd_line)
    {
        CommandSeq seq(user_cmd_line);

        enum EIdents { eCodec, eAudio, eFragment, eTrack, eSession, ePlayback, eRender, eNone };
        IdentsMap<EIdents> idents_map{ {"codec", eCodec }, {"audio", eAudio}, {"fragment", eFragment}, {"track", eTrack},
                                       {"session", eSession}, {"playback", ePlayback}, {"render", eRender} };

        std::string token = seq.sliceNextToken();

        EIdents cmd = idents_map.hasIdent(token) ?
            idents_map.getIdent(token) : eNone;

        switch (cmd)
        {
        case eCodec:
            return cmdCodec(seq);

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