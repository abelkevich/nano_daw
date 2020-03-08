#include "api_render.h"
#include "rt_output.h"
#include "sessions_manager.h"

namespace ClientAPI
{
    json cmdPlayback(CommandSeq seq)
    {
        enum EIdents { ePlay, ePause, eStop, eInit, eRender, eNone };
        IdentsMap<EIdents> idents_map{ {"play", ePlay}, {"pause", ePause}, {"stop", eStop},
                                       {"init", eInit}, {"render", eRender} };

        std::string token = seq.sliceNextToken();

        EIdents cmd = idents_map.hasIdent(token) ?
            idents_map.getIdent(token) : eNone;

        switch (cmd)
        {
        case ePlay:
        {
            if (!seq.hasNTokens(3))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            const id_t session_id = stoi(seq.sliceNextToken());
            uint32_t from_ms = stoi(seq.sliceNextToken());
            uint32_t to_ms = stoi(seq.sliceNextToken());

            if (!SessionsManager::getSession(session_id))
            {
                return jsonErrResponse(EErrCodes::eInvalidSession);
            }

            if (Playback::play(session_id, from_ms, to_ms) != 0)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        case ePause:
        {
            if (Playback::pause() != 0)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        case eStop:
        {
            if (Playback::stop() != 0)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        }
        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}