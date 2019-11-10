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
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			if (!g_session)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_session);
			}

			std::string mix_path = seq.sliceNextToken();

			if (render(mix_path) != 0)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
			}

			return APIResponse(EKernelAPIStatus::eOk);
		}
		default:
			break;
		}

		return APIResponse(EKernelAPIStatus::eErr, c_err_cannot_find_command);
	}
}