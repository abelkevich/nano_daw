#include "api_render.h"
#include "codec_manager.h"

namespace ClientAPI
{
    json cmdCodec(CommandSeq seq)
	{
		enum EIdents { eInit, eList, eNone };
        IdentsMap<EIdents> idents_map{ {"init", eInit}, {"list", eList}, {"none", eNone} };
        
        std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;

        switch (cmd)
        {
        case eInit:
        {
            if (CodecManager::initCodecs() != 0)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json({ {"status", "ok"} });
        }
        case eList:
        {
            auto codecs = CodecManager::getInitedCodecs();

            json::array_t response = json::array();

            for (auto c: codecs)
            {
                json arr_elem({ {"name", c.lib_name} });
                response.push_back(arr_elem);
            }

			return response;
        }
        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
	}
}