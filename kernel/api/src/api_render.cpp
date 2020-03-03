#include "api_render.h"
#include "render.h"

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
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            if (!g_session)
            {
                return jsonErrResponse(EErrCodes::eInvalidSession);
            }

            std::string mix_path = seq.sliceNextToken();

            if (!render(mix_path))
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}