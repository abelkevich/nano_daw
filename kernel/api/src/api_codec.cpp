#include "api_render.h"
#include "codec_manager.h"

namespace ClientAPI
{
	EKernelAPIStatus cmdCodec(CommandSeq seq)
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
                sendToClient("Failed to init codecs");
                return EKernelAPIStatus::eErr;
            }

            sendToClient("Codecs inited");
            return EKernelAPIStatus::eOk;
        }
        case eList:
        {
            auto codecs = CodecManager::getInitedCodecs();

            std::stringstream sstream;

            for (auto c: codecs)
            {
                sstream << "Lib: " << c.lib_name;
                sstream << std::endl;
            }

            sendToClient(sstream.str());

            return EKernelAPIStatus::eOk;
        }
        }

		return EKernelAPIStatus::eErr;
	}
}