#include "api_track.h"
#include "items_manager.h"

namespace ClientAPI
{
	APIResponse cmdTrack(CommandSeq seq)
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
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
                return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
            }


            std::stringstream sstream;

            sstream << "name: " << track->getName() << ";";
            sstream << "solo: " << track->getSolo() << ";";
            sstream << "mute: " << track->getMute() << ";";
            sstream << "gain: " << track->getGain() << ";";
            sstream << "level: " << track->getLevel() << ";";
            sstream << "fragments: [";

            for (id_t id : track->getFragments())
            {
                sstream << id << ",";
            }

            sstream << "];";

			return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }

		case eList:
		{

			std::set<id_t> track_ids = ItemsManager::getTracks();

			std::stringstream sstream;

			sstream << '[';

			for (id_t id : track_ids)
			{
				Track *track = ItemsManager::getTrack(id);

				if (!track)
				{
					return APIResponse(EKernelAPIStatus::eErr, sstream.str());
				}

				sstream << "id: " << id;
				sstream << " name: " << track->getName();
				sstream << "; ";
			}

			sstream << ']';

			return APIResponse(EKernelAPIStatus::eOk, sstream.str());
		}

		case eAdd:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string name = seq.sliceNextToken();

			id_t id = ItemsManager::createTrack(name);

			if (!id)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
			}

			return APIResponse(EKernelAPIStatus::eOk, "id: " + std::to_string(id));
		}

		case eRemove:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			if (!ItemsManager::getTrack(id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

			if (!ItemsManager::removeTrack(id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
			}

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eLink:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			std::string fragment_id_str = seq.sliceNextToken();
			id_t fragment_id = stoi(fragment_id_str);

            Track* track = ItemsManager::getTrack(track_id);

			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

			if (!ItemsManager::getFragment(fragment_id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

			if (track->linkFragment(fragment_id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
			}

			return APIResponse(EKernelAPIStatus::eOk);
		}

        case eUnlink:
        {
            if (!seq.hasNTokens(2))
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
            }

            std::string track_id_str = seq.sliceNextToken();
            id_t track_id = stoi(track_id_str);

            std::string fragment_id_str = seq.sliceNextToken();
            id_t fragment_id = stoi(fragment_id_str);

            Track* track = ItemsManager::getTrack(track_id);

			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

			if (!ItemsManager::getFragment(fragment_id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            if (track->unlinkFragment(fragment_id))
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
            }

			return APIResponse(EKernelAPIStatus::eOk);
        }

		case eMute:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            track->setMute(!track->getMute());

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eSolo:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            track->setSolo(!track->getSolo());

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eVolume:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string volume_str = seq.sliceNextToken();
			int volume = stoi(volume_str);

			if (volume > 100 || volume < 0)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_arg_value);
			}

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            track->setLevel(volume);

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eGain:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);

			std::string gain_str = seq.sliceNextToken();
			int gain = stoi(gain_str);

			if (gain < 0)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_arg_value);
			}

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            track->setGain(gain);

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case ePan:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string pan_str = seq.sliceNextToken();
			int pan = stoi(pan_str);

			if (pan < 100 || pan > 100)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_arg_value);
			}

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            track->setPan(pan);

			return APIResponse(EKernelAPIStatus::eOk);
		}

        case eName:
        {
            if (!seq.hasNTokens(2))
            {
                return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string name_str = seq.sliceNextToken();
            
			if (name_str.size() > 30)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_arg_value);
			}

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
            }

            track->setName(name_str);

			return APIResponse(EKernelAPIStatus::eOk);
        }

		}

		return APIResponse(EKernelAPIStatus::eErr, c_err_cannot_find_command);
	}
}