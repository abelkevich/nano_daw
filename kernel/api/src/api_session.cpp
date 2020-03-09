#include "api_session.h"
#include "sessions_manager.h"
#include "tracks_manager.h"

namespace ClientAPI
{
    json cmdSession(CommandSeq seq)
    {
        enum EIdents { eAdd, eRemove, eInfo, eList, eNone };
        IdentsMap<EIdents> idents_map{ {"add", eAdd}, {"remove", eRemove}, {"list", eList}, {"info", eInfo} };

        std::string token = seq.sliceNextToken();

        EIdents cmd = idents_map.hasIdent(token) ?
            idents_map.getIdent(token) : eNone;


        switch (cmd)
        {
        case eInfo:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            id_t id = stoi(seq.sliceNextToken());

            Session* session = SessionsManager::getSession(id);
            if (!session)
            {
                return jsonErrResponse(EErrCodes::eInvalidSession);
            }

            return json({ {"id", session->getId()}, {"name", session->getName()}, {"path", session->getPath()}, {"sample rate", session->getSampleRate()}
                        , {"tracks", session->getTracks()}});
        }

        case eList:
        {
            std::set<id_t> sessions_id = SessionsManager::getSessions();

            json::array_t response = json::array();
            for (const id_t id : sessions_id)
            {
                json arr_elem({ {"id", id} });
                response.push_back(arr_elem);
            }

            return response;
        }

        case eAdd:
        {
            if (!seq.hasNTokens(3))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            const std::string name = seq.sliceNextToken();
            const std::string path = seq.sliceNextToken();
            const freq_t smp_rate = stoi(seq.sliceNextToken());

            const id_t session_id = SessionsManager::createSession(name, path, smp_rate);

            if (!session_id)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json({ {"id", session_id} });
        }

        case eRemove:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            const id_t session_id = stoi(seq.sliceNextToken());

            if (!SessionsManager::getSession(session_id))
            {
                return jsonErrResponse(EErrCodes::eInvalidSession);
            }

            if (!SessionsManager::removeSession(session_id))
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }

        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}