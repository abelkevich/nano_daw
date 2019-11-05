#include "api_session.h"
#include "items_manager.h"

namespace ClientAPI
{
	EKernelAPIStatus cmdSession(CommandSeq seq)
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
            sendToClient("Err! Unimplemented method");
            return EKernelAPIStatus::eErr;
		}

		case eLink:
		{
			if (!seq.hasNTokens(1))
			{
				sendToClient("Err! Invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			if (!ItemsManager::getTrack(track_id))
			{
				sendToClient("Err! Cannot find track by id");
				return EKernelAPIStatus::eErr;
			}

			g_session->tracks.insert(track_id);

			sendToClient("Track linked");
			return EKernelAPIStatus::eOk;
		}

		case eCreate:
		{
			if (!seq.hasNTokens(3))
			{
				sendToClient("Err! Invalid parameters");
				return EKernelAPIStatus::eErr;
			}

			std::string name = seq.sliceNextToken();
			std::string path = seq.sliceNextToken();
			std::string smp_rate_str = seq.sliceNextToken();

			uint32_t smp_rate = stoi(smp_rate_str);

			if (smp_rate != 48000)
			{
				sendToClient("Err! Supporting only 48kHz format");
				return EKernelAPIStatus::eErr;
			}

			g_session = new Session(name, path, smp_rate);

            sendToClient("Session created!");
			return EKernelAPIStatus::eOk;
		}

		case eSave:
        {
            sendToClient("Err! Unimplemented method");
            return EKernelAPIStatus::eErr;
        }
		}

        sendToClient("Err! Cannot find such command in 'session' section");
		return EKernelAPIStatus::eErr;
	}
}