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
            if (!CodecManager::initCodecs())
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }
        case eList:
        {
            auto codecs = CodecManager::getInitedCodecs();

            json::array_t response = json::array();

            for (auto c: codecs)
            {
                std::string name(c->getName());
                std::string ext(c->getExtensions());

                json arr_elem({ {"name", name}, {"extensions", ext } });
                response.push_back(arr_elem);
            }

            return response;
        }
        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}