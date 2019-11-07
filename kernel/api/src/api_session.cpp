#include "api_session.h"
#include "items_manager.h"

namespace ClientAPI
{
	APIResponse cmdSession(CommandSeq seq)
	{
		enum EIdents { eLoad, eCreate, eSave, eLink, eNone };
        IdentsMap<EIdents> idents_map{ {"load", eLoad }, {"create", eCreate}, {"save", eSave}, {"link", eLink} };

		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;


		switch (cmd)
		{
		case eLoad:
		{
            return APIResponse(EKernelAPIStatus::eErr, "Unimplemented method");
		}

		case eLink:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			if (!ItemsManager::getTrack(track_id))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
			}

			g_session->tracks.insert(track_id);

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eCreate:
		{
			if (!seq.hasNTokens(3))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string name = seq.sliceNextToken();
			std::string path = seq.sliceNextToken();
			std::string smp_rate_str = seq.sliceNextToken();

			uint32_t smp_rate = stoi(smp_rate_str);

			if (smp_rate != 48000)
			{
				return APIResponse(EKernelAPIStatus::eErr, "Supporting only 48kHz format");
			}

			g_session = new Session(name, path, smp_rate);

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eSave:
        {
			return APIResponse(EKernelAPIStatus::eErr, "Unimplemented method");
        }
		}

		return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command in 'session' section");
	}
}