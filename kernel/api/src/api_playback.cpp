#include "api_render.h"
#include "rt_output.h"

namespace ClientAPI
{
    json cmdPlayback(CommandSeq seq)
	{
        enum EIdents { eSession, eNone };
        IdentsMap<EIdents> idents_map{ {"session", eSession}};

        std::string token = seq.sliceNextToken();

        EIdents cmd = idents_map.hasIdent(token) ?
            idents_map.getIdent(token) : eNone;

        switch (cmd)
        {
        case eSession:
        {
            play_session();
            return json();
        }
        }
        return jsonErrResponse(EErrCodes::eCommandNotFound);
	}
}