#include "api_render.h"
#include "render.h"
#include "sessions_manager.h"

namespace ClientAPI
{
    json cmdRender(CommandSeq seq)
    {
        enum EIdents { eNone, eSession };
        IdentsMap<EIdents> idents_map{ {"session", eSession}, {"none", eNone} };
        std::string token = seq.sliceNextToken();

        EIdents cmd = idents_map.hasIdent(token) ?
            idents_map.getIdent(token) : eNone;

        switch (cmd)
        {
        case eSession:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            const id_t session_id = stoi(seq.sliceNextToken());
            const std::string mix_path = seq.sliceNextToken();

            if (!SessionsManager::getSession(session_id))
            {
                return jsonErrResponse(EErrCodes::eInvalidSession);
            }

            if (!render(session_id, mix_path))
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}