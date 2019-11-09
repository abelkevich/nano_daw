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
				return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
            }

            return APIResponse(EKernelAPIStatus::eOk);
        }
        case eList:
        {
            auto codecs = CodecManager::getInitedCodecs();

            std::stringstream sstream;

			sstream << "[";

            for (auto c: codecs)
            {
                sstream << "name: " << c.lib_name << ";";
            }

			sstream << "]";

			return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }
        }

		return APIResponse(EKernelAPIStatus::eErr, c_err_cannot_find_command);
	}
}