#include "api_render.h"
#include "render.h"

namespace ClientAPI
{
	APIResponse cmdRender(CommandSeq seq)
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
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string mix_path = seq.sliceNextToken();

			if (render(*g_session, mix_path) != 0)
			{
				return APIResponse(EKernelAPIStatus::eErr);
			}

			return APIResponse(EKernelAPIStatus::eOk);
		}
		default:
			break;
		}

		return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command in 'render' section");
	}
}