#include "api_render.h"
#include "codec_manager.h"

namespace ClientAPI
{
	APIResponse cmdCodec(CommandSeq seq)
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
				return APIResponse(EKernelAPIStatus::eErr, "Failed to init codecs");
            }

            return APIResponse(EKernelAPIStatus::eOk, "Codecs inited");
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

			return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }
        }

		return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command in 'codec' section");
	}
}