#include "api_track.h"
#include "items_manager.h"

namespace ClientAPI
{
    json cmdTrack(CommandSeq seq)
    {
        enum EIdents { eList, eAdd, eRemove, eMute, eSolo, 
                       eVolume, eGain, ePan, eLink, eUnlink,
                       eName, eInfo, eNone };
        IdentsMap<EIdents> idents_map{ {"add", eAdd}, {"remove", eRemove}, {"mute", eMute},
                                       {"solo", eSolo}, {"volume", eVolume}, {"gain", eGain},
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

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            return json({ {"name", track->getName()}, {"solo", track->getSolo()}, {"mute", track->getMute()}
                        , {"gain", track->getGain()}, {"level", track->getLevel()}, {"fragments", track->getFragments()} });
        }

        case eList:
        {
            std::set<id_t> track_ids = ItemsManager::getTracks();

            json::array_t response = json::array();

            for (id_t id : track_ids)
            {
                Track *track = ItemsManager::getTrack(id);

                if (!track)
                {
                    return jsonErrResponse(EErrCodes::eOperationFailed);
                }

                json arr_elem({ {"id", id}, {"name", track->getName()} });
                response.push_back(arr_elem);
            }

            return response;
        }

        case eAdd:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string name = seq.sliceNextToken();

            id_t id = ItemsManager::createTrack(name);

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

            if (!ItemsManager::getTrack(id))
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            if (!ItemsManager::removeTrack(id))
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }

        case eLink:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string track_id_str = seq.sliceNextToken();
            id_t track_id = stoi(track_id_str);

            std::string fragment_id_str = seq.sliceNextToken();
            id_t fragment_id = stoi(fragment_id_str);

            Track* track = ItemsManager::getTrack(track_id);

            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            if (!ItemsManager::getFragment(fragment_id))
            {
                return jsonErrResponse(EErrCodes::eInvalidFragment);
            }

            if (track->linkFragment(fragment_id))
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json();
        }

        case eUnlink:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string track_id_str = seq.sliceNextToken();
            id_t track_id = stoi(track_id_str);

            std::string fragment_id_str = seq.sliceNextToken();
            id_t fragment_id = stoi(fragment_id_str);

            Track* track = ItemsManager::getTrack(track_id);

            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            if (!ItemsManager::getFragment(fragment_id))
            {
                return jsonErrResponse(EErrCodes::eInvalidFragment);
            }

            if (track->unlinkFragment(fragment_id))
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

            Track* track = ItemsManager::getTrack(id);

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

            Track* track = ItemsManager::getTrack(id);

            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setSolo(!track->getSolo());

            return json();
        }

        case eVolume:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string volume_str = seq.sliceNextToken();
            int volume = stoi(volume_str);

            if (volume > 100 || volume < 0)
            {
                return jsonErrResponse(EErrCodes::eInvalidArgValue);
            }

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
                return jsonErrResponse(EErrCodes::eInvalidTrack);
            }

            track->setLevel(volume);

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

            Track* track = ItemsManager::getTrack(id);
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

            Track* track = ItemsManager::getTrack(id);
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

            Track* track = ItemsManager::getTrack(id);
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