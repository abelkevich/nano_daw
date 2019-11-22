#include "api_render.h"
#include "rt_output.h"

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
            if (Playback::play() != 0)
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
        case eInit:
        {
            if (Playback::init() != 0)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        case eRender:
        {
            if (Playback::render_selection(0, 0) != 0)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        }
        return jsonErrResponse(EErrCodes::eCommandNotFound);
	}
}