#include "api_session.h"
#include "items_manager.h"

namespace ClientAPI
{
    json cmdSession(CommandSeq seq)
	{
		enum EIdents { eLoad, eCreate, eSave, eLink, eInfo, eNone };
        IdentsMap<EIdents> idents_map{ {"load", eLoad }, {"create", eCreate}, {"save", eSave},
                                       {"link", eLink}, {"info", eInfo} };

		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;


		switch (cmd)
		{
        case eInfo:
        {
            if (!g_session)
            {
                return json({ {"error", { {"code", c_err_invalid_session_code}, {"msg", c_err_invalid_session_str}}} });
            }

            return json({ {"name", g_session->getName()}, {"path", g_session->getPath()}, {"sample rate", g_session->getSampleRate()}
                        , {"tracks", g_session->getTracks()}});
        }

		case eLoad:
		{
            return json({ {"error", { {"code", c_err_unimplemented_method_code}, {"msg", c_err_unimplemented_method_str}}} });
		}

		case eLink:
		{
			if (!seq.hasNTokens(1))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			if (!g_session)
			{
                return json({ {"error", { {"code", c_err_invalid_session_code}, {"msg", c_err_invalid_session_str}}} });
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			if (!ItemsManager::getTrack(track_id))
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

			g_session->linkTrack(track_id);

            return json({ {"name", g_session->getName()} });
		}

		case eCreate:
		{
			if (!seq.hasNTokens(3))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string name = seq.sliceNextToken();
			std::string path = seq.sliceNextToken();
			std::string smp_rate_str = seq.sliceNextToken();

			uint32_t smp_rate = stoi(smp_rate_str);

			if (smp_rate != 48000)
			{
                return json({ {"error", { {"code", c_err_invalid_arg_value_code}, {"msg", c_err_invalid_arg_value_str}}} });
			}

			g_session = new Session(name, path, smp_rate);

            return json({ {"name", name} });
		}

		case eSave:
        {
            return json({ {"error", { {"code", c_err_unimplemented_method_code}, {"msg", c_err_unimplemented_method_str}}} });
        }
		}

        return json({ {"error", { {"code", c_err_cannot_find_command_code}, {"msg", c_err_cannot_find_command_str}}} });
	}
}