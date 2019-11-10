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
            return APIResponse(EKernelAPIStatus::eErr, c_err_unimplemented_method);
		}

		case eLink:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			if (!g_session)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_session);
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			if (!ItemsManager::getTrack(track_id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

			g_session->linkTrack(track_id);

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eCreate:
		{
			if (!seq.hasNTokens(3))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string name = seq.sliceNextToken();
			std::string path = seq.sliceNextToken();
			std::string smp_rate_str = seq.sliceNextToken();

			uint32_t smp_rate = stoi(smp_rate_str);

			if (smp_rate != 48000)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_arg_value);
			}

			g_session = new Session(name, path, smp_rate);

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eSave:
        {
			return APIResponse(EKernelAPIStatus::eErr, c_err_unimplemented_method);
        }
		}

		return APIResponse(EKernelAPIStatus::eErr, c_err_cannot_find_command);
	}
}