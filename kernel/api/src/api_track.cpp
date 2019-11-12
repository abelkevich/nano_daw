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
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
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
                    return json({ {"error", { {"code", c_err_operation_failed_code}, {"msg", c_err_operation_failed_str}}} });
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
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string name = seq.sliceNextToken();

			id_t id = ItemsManager::createTrack(name);

			if (!id)
			{
                return json({ {"error", { {"code", c_err_operation_failed_code}, {"msg", c_err_operation_failed_str}}} });
			}

            return json({ {"id", id} });
		}

		case eRemove:
		{
			if (!seq.hasNTokens(1))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			if (!ItemsManager::getTrack(id))
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

			if (!ItemsManager::removeTrack(id))
			{
                return json({ {"error", { {"code", c_err_operation_failed_code}, {"msg", c_err_operation_failed_str}}} });
			}

            return json({ {"id", id} });
		}

		case eLink:
		{
			if (!seq.hasNTokens(2))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			std::string fragment_id_str = seq.sliceNextToken();
			id_t fragment_id = stoi(fragment_id_str);

            Track* track = ItemsManager::getTrack(track_id);

			if (!track)
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

			if (!ItemsManager::getFragment(fragment_id))
			{
                return json({ {"error", { {"code", c_err_invalid_fragment_id_code}, {"msg", c_err_invalid_fragment_id_str}}} });
			}

			if (track->linkFragment(fragment_id))
			{
                return json({ {"error", { {"code", c_err_operation_failed_code}, {"msg", c_err_operation_failed_str}}} });
			}

            return json({ {"id", track_id_str} });
		}

        case eUnlink:
        {
            if (!seq.hasNTokens(2))
            {
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
            }

            std::string track_id_str = seq.sliceNextToken();
            id_t track_id = stoi(track_id_str);

            std::string fragment_id_str = seq.sliceNextToken();
            id_t fragment_id = stoi(fragment_id_str);

            Track* track = ItemsManager::getTrack(track_id);

			if (!track)
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

			if (!ItemsManager::getFragment(fragment_id))
			{
                return json({ {"error", { {"code", c_err_invalid_fragment_id_code}, {"msg", c_err_invalid_fragment_id_str}}} });
			}

            if (track->unlinkFragment(fragment_id))
            {
                return json({ {"error", { {"code", c_err_operation_failed_code}, {"msg", c_err_operation_failed_str}}} });
            }

            return json({ {"id", track_id_str} });
        }

		case eMute:
		{
			if (!seq.hasNTokens(1))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

            track->setMute(!track->getMute());

            return json({ {"id", id} });
		}

		case eSolo:
		{
			if (!seq.hasNTokens(1))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

            track->setSolo(!track->getSolo());

            return json({ {"id", id} });
		}

		case eVolume:
		{
			if (!seq.hasNTokens(2))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string volume_str = seq.sliceNextToken();
			int volume = stoi(volume_str);

			if (volume > 100 || volume < 0)
			{
                return json({ {"error", { {"code", c_err_invalid_arg_value_code}, {"msg", c_err_invalid_arg_value_str}}} });
			}

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

            track->setLevel(volume);

            return json({ {"id", id} });
		}

		case eGain:
		{
			if (!seq.hasNTokens(2))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);

			std::string gain_str = seq.sliceNextToken();
			int gain = stoi(gain_str);

			if (gain < 0)
			{
                return json({ {"error", { {"code", c_err_invalid_arg_value_code}, {"msg", c_err_invalid_arg_value_str}}} });
			}

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

            track->setGain(gain);

            return json({ {"id", id} });
		}

		case ePan:
		{
			if (!seq.hasNTokens(2))
			{
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string pan_str = seq.sliceNextToken();
			int pan = stoi(pan_str);

			if (pan < 100 || pan > 100)
			{
                return json({ {"error", { {"code", c_err_invalid_arg_value_code}, {"msg", c_err_invalid_arg_value_str}}} });
			}

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
			}

            track->setPan(pan);

            return json({ {"id", id} });
		}

        case eName:
        {
            if (!seq.hasNTokens(2))
            {
                return json({ {"error", { {"code", c_err_invalid_args_number_code}, {"msg", c_err_invalid_args_number_str}}} });
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string name_str = seq.sliceNextToken();
            
			if (name_str.size() > 30)
			{
                return json({ {"error", { {"code", c_err_invalid_arg_value_code}, {"msg", c_err_invalid_arg_value_str}}} });
			}

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
                return json({ {"error", { {"code", c_err_invalid_track_id_code}, {"msg", c_err_invalid_track_id_str}}} });
            }

            track->setName(name_str);

            return json({ {"id", id} });
        }

		}

        return json({ {"error", { {"code", c_err_cannot_find_command_code}, {"msg", c_err_cannot_find_command_str}}} });
	}
}