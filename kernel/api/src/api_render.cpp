#include "api_render.h"

namespace ClientAPI
{
	EKernelAPIStatus cmdRender(CommandSeq seq)
	{
		enum EIdents { eNone, eAll };
		IdentsMap<EIdents> idents_map{ {"all", eAll}, {"all", eAll} };
		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;

		switch (cmd)
		{
		case eAll:
		{
			if (!seq.hasNTokens(1))
			{
				sendToClient("err: invalid syntax for render all");

				return EKernelAPIStatus::eOk;
			}

			std::string mix_name = seq.sliceNextToken();


			sendToClient("err: rendered");

			break;
		}
		default:
			break;
		}

		return EKernelAPIStatus::eErr;
	}
}