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
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
                return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
            }


            std::stringstream sstream;

            sstream << "name: " << track->name << ";";
            sstream << "solo: " << track->solo << ";";
            sstream << "mute: " << track->mute << ";";
            sstream << "gain: " << track->gain << ";";
            sstream << "level: " << track->level << ";";
            sstream << "fragments: [";

            for (id_t id : track->fragments)
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

			for (id_t id : track_ids)
			{
				Track *track = ItemsManager::getTrack(id);

				if (!track)
				{
					return APIResponse(EKernelAPIStatus::eErr, sstream.str());
				}

				sstream << "Track: id: " << id;
				sstream << " name: " << track->name;
				sstream << std::endl;
			}

			return APIResponse(EKernelAPIStatus::eOk, sstream.str());
		}

		case eAdd:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string name = seq.sliceNextToken();

			id_t id = ItemsManager::createTrack(name);

			if (!id)
			{
				return APIResponse(EKernelAPIStatus::eErr);
			}

			return APIResponse(EKernelAPIStatus::eOk, "id: " + std::to_string(id));
		}

		case eRemove:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			if (!ItemsManager::removeTrack(id))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
			}

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eLink:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			std::string fragment_id_str = seq.sliceNextToken();
			id_t fragment_id = stoi(fragment_id_str);

			if (ItemsManager::linkFragmentToTrack(track_id, fragment_id))
			{
				return APIResponse(EKernelAPIStatus::eErr);
			}

			return APIResponse(EKernelAPIStatus::eOk);
		}

        case eUnlink:
        {
            if (!seq.hasNTokens(2))
            {
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
            }

            std::string track_id_str = seq.sliceNextToken();
            id_t track_id = stoi(track_id_str);

            std::string fragment_id_str = seq.sliceNextToken();
            id_t fragment_id = stoi(fragment_id_str);

            if (ItemsManager::unlinkFragmentFromTrack(track_id, fragment_id))
            {
				return APIResponse(EKernelAPIStatus::eErr);
            }

			return APIResponse(EKernelAPIStatus::eOk);
        }

		case eMute:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
			}

			track->mute = !track->mute;

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eSolo:
		{
			if (!seq.hasNTokens(1))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
			}

			track->solo = !track->solo;

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eVolume:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string volume_str = seq.sliceNextToken();
			uint32_t volume = stoi(volume_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
			}

			track->level = volume;

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case eGain:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);

			std::string gain_str = seq.sliceNextToken();
			uint32_t gain = stoi(gain_str);

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
			}

			track->gain = gain;

			return APIResponse(EKernelAPIStatus::eOk);
		}

		case ePan:
		{
			if (!seq.hasNTokens(2))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string pan_str = seq.sliceNextToken();
			uint32_t pan = stoi(pan_str);

			Track* track = ItemsManager::getTrack(id);
			if (!track)
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
			}

			track->pan = pan;

			return APIResponse(EKernelAPIStatus::eOk);
		}

        case eName:
        {
            if (!seq.hasNTokens(2))
            {
                return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string name_str = seq.sliceNextToken();
            
            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
            }

            track->name = name_str;

			return APIResponse(EKernelAPIStatus::eOk);
        }

		}

		return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command in 'track' section");
	}
}