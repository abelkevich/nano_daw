#include "api_session.h"

namespace ClientAPI
{
	EKernelAPIStatus cmdSession(CommandSeq seq)
	{
		enum EIdents { eLoad, eCreate, eSave, eNone };
		IdentsMap<EIdents> idents_map{ {"load", eLoad }, {"create", eCreate}, {"save", eSave} };

		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;


		switch (cmd)
		{
		case eLoad:
		{
			
			break;
		}
		case eCreate:
		{
			if (!seq.hasNTokens(3))
			{
				sendToClient("invalid parameters");
				return EKernelAPIStatus::eErr;
			}

			std::string name = seq.sliceNextToken();
			std::string path = seq.sliceNextToken();
			std::string smp_rate_str = seq.sliceNextToken();

			uint32_t smp_rate = stoi(smp_rate_str);

			if (smp_rate != 48000)
			{
				sendToClient("supporting only 48kHz format");
				return EKernelAPIStatus::eErr;
			}

			status_t status = 0; //createSession(name, path, smp_rate);

			if (status != 0)
			{
				sendToClient("Warn: command exited with code: " + std::to_string(status));
				return EKernelAPIStatus::eWarn;
			}

			return EKernelAPIStatus::eOk;
		}
		break;

		case eSave:
			break;
		}

		return EKernelAPIStatus::eErr;
	}
}