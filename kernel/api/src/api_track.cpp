#include "api_track.h"
#include "tracks_manager.h"
#include "fragments_manager.h"

namespace ClientAPI
{
    json cmdTrack(CommandSeq seq)
    {
        enum EIdents { eList, eAdd, eRemove, eMute, eSolo, 
                       eLevel, eGain, ePan, eLink, eUnlink,
                       eName, eInfo, eNone };
        IdentsMap<EIdents> idents_map{ {"add", eAdd}, {"remove", eRemove}, {"mute", eMute},
                                       {"solo", eSolo}, {"level", eLevel}, {"gain", eGain},
                                       {"pan", ePan}, {"list", eList}, {"link", eLink}, 
                                       {"unlink", eUnlink}, {"info", eInfo}, {"none", eNone} };

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

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Track* track = TracksManager::getTrack(id);
            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            return json({ {"id", track->getId()}, {"session", track->getSessionId()}, {"name", track->getName()}, {"solo", track->getSolo()}, {"mute", track->getMute()}
                        , {"gain", track->getGain()}, {"level", track->getLevel()}, {"fragments", track->getFragments()} });
        }

        case eList:
        {
            std::set<id_t> track_ids = TracksManager::getTracks();

            json::array_t response = json::array();

            for (id_t id : track_ids)
            {
                Track *track = TracksManager::getTrack(id);

                if (!track)
                {
                    return jsonErrResponse(EErrCodes::eOperationFailed);
                }

                json arr_elem({ {"id", id} });
                response.push_back(arr_elem);
            }

            return response;
        }

        case eAdd:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            const id_t session_id = stoi(seq.sliceNextToken());
            const std::string name = seq.sliceNextToken();

            const id_t id = TracksManager::createTrack(session_id, name);

            if (!id)
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json({ {"id", id} });
        }

        case eRemove:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();

            id_t id = stoi(id_str);

            if (!TracksManager::getTrack(id))
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            if (!TracksManager::removeTrack(id))
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }

        case eMute:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();

            id_t id = stoi(id_str);

            Track* track = TracksManager::getTrack(id);

            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setMute(!track->getMute());

            return json();
        }

        case eSolo:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();

            id_t id = stoi(id_str);

            Track* track = TracksManager::getTrack(id);

            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setSolo(!track->getSolo());

            return json();
        }

        case eLevel:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string level_str = seq.sliceNextToken();
            int level = stoi(level_str);

            if (level > 100 || level < 0)
            {
                return jsonErrResponse(EErrCodes::eInvalidArgValue);
            }

            Track* track = TracksManager::getTrack(id);
            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setLevel(level);

            return json();
        }

        case eGain:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            std::string gain_str = seq.sliceNextToken();
            int gain = stoi(gain_str);

            if (gain < 0)
            {
                return jsonErrResponse(EErrCodes::eInvalidArgValue);
            }

            Track* track = TracksManager::getTrack(id);
            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setGain(gain);

            return json();
        }

        case ePan:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string pan_str = seq.sliceNextToken();
            int pan = stoi(pan_str);

            if (pan < 100 || pan > 100)
            {
                return jsonErrResponse(EErrCodes::eInvalidArgValue);
            }

            Track* track = TracksManager::getTrack(id);
            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setPan(pan);

            return json();
        }

        case eName:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string name_str = seq.sliceNextToken();
            
            if (name_str.size() > 30)
            {
                return jsonErrResponse(EErrCodes::eInvalidArgValue);
            }

            Track* track = TracksManager::getTrack(id);
            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setName(name_str);

            return json();
        }

        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}