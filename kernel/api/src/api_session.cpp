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
                return jsonErrResponse(EErrCodes::eInvalidSession);
            }

            return json({ {"name", g_session->getName()}, {"path", g_session->getPath()}, {"sample rate", g_session->getSampleRate()}
                        , {"tracks", g_session->getTracks()}});
        }

		case eLoad:
		{
            return jsonErrResponse(EErrCodes::eUnimplementedMethod);
		}

		case eLink:
		{
			if (!seq.hasNTokens(1))
			{
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
			}

			if (!g_session)
			{
                return jsonErrResponse(EErrCodes::eInvalidSession);
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			if (!ItemsManager::getTrack(track_id))
			{
                return jsonErrResponse(EErrCodes::eInvalidTrack);
			}

			g_session->linkTrack(track_id);

            return json();
		}

		case eCreate:
		{
			if (!seq.hasNTokens(3))
			{
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
			}

			std::string name = seq.sliceNextToken();
			std::string path = seq.sliceNextToken();
			std::string smp_rate_str = seq.sliceNextToken();

			uint32_t smp_rate = stoi(smp_rate_str);

			if (smp_rate != 48000)
			{
                return jsonErrResponse(EErrCodes::eInvalidArgValue);
			}

			g_session = new Session(name, path, smp_rate);

            return json();
		}

		case eSave:
        {
            return jsonErrResponse(EErrCodes::eUnimplementedMethod);
        }
		}

        return jsonErrResponse(EErrCodes::eCommandNotFound);
	}
}