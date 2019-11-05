#include "api_render.h"
#include "render.h"

namespace ClientAPI
{
	EKernelAPIStatus cmdRender(CommandSeq seq)
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
				sendToClient("err: invalid syntax");
				return EKernelAPIStatus::eOk;
			}

			std::string mix_path = seq.sliceNextToken();

			if (!render(*g_session, mix_path))
			{
				sendToClient("err: render");
				return EKernelAPIStatus::eErr;
			}

			sendToClient("Session rendered");
			return EKernelAPIStatus::eOk;
		}
		default:
			break;
		}

		return EKernelAPIStatus::eErr;
	}
}