#include "api_track.h"
#include "items_manager.h"

namespace ClientAPI
{
	EKernelAPIStatus cmdTrack(CommandSeq seq)
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
                sendToClient("Err: invalid args");
                return EKernelAPIStatus::eErr;
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Track* track = ItemsManager::getTrack(id);
            if (!track)
            {
                sendToClient("Err! Cannot find track by id");
                return EKernelAPIStatus::eErr;
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

            sendToClient(sstream.str());

            return EKernelAPIStatus::eOk;
        }

		case eList:
		{

			std::set<id_t> track_ids = ItemsManager::getTracks();

			std::stringstream sstream;

			for (id_t id : track_ids)
			{
				Track *track = ItemsManager::getTrack(id);

				sstream << "Track: id: " << id;
				sstream << " name: " << track->name;
				sstream << std::endl;
			}

			sendToClient(sstream.str());

			return EKernelAPIStatus::eOk;
		}

		case eAdd:
		{
			if (!seq.hasNTokens(1))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string name = seq.sliceNextToken();

			id_t id = ItemsManager::createTrack(name);

			if (!id)
			{
				sendToClient("Err! Cannot create track");
				return EKernelAPIStatus::eErr;
			}

			sendToClient("Track added with id: " + std::to_string(id));
			return EKernelAPIStatus::eOk;
		}

		case eRemove:
		{
			if (!seq.hasNTokens(1))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			if (!ItemsManager::removeTrack(id))
			{
				sendToClient("Err! Cannot find track by id");
				return EKernelAPIStatus::eErr;
			}

			sendToClient("Track removed");
			return EKernelAPIStatus::eOk;
		}

		case eLink:
		{
			if (!seq.hasNTokens(2))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string track_id_str = seq.sliceNextToken();
			id_t track_id = stoi(track_id_str);

			std::string fragment_id_str = seq.sliceNextToken();
			id_t fragment_id = stoi(fragment_id_str);

			if (ItemsManager::linkFragmentToTrack(track_id, fragment_id))
			{
				sendToClient("Err! Cannot link");
				return EKernelAPIStatus::eErr;
			}

			sendToClient("Track linked");
			return EKernelAPIStatus::eOk;
		}

        case eUnlink:
        {
            if (!seq.hasNTokens(2))
            {
                sendToClient("Err: invalid args");
                return EKernelAPIStatus::eErr;
            }

            std::string track_id_str = seq.sliceNextToken();
            id_t track_id = stoi(track_id_str);

            std::string fragment_id_str = seq.sliceNextToken();
            id_t fragment_id = stoi(fragment_id_str);

            if (ItemsManager::unlinkFragmentFromTrack(track_id, fragment_id))
            {
                sendToClient("Err! Cannot unlink");
                return EKernelAPIStatus::eErr;
            }

            sendToClient("Track unlinked");
            return EKernelAPIStatus::eOk;
        }

		case eMute:
		{
			if (!seq.hasNTokens(1))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				sendToClient("Err! Cannot find track by id");
				return EKernelAPIStatus::eErr;
			}

			track->mute = !track->mute;

			sendToClient("Track mute inverted");
			return EKernelAPIStatus::eOk;
			break;
		}

		case eSolo:
		{
			if (!seq.hasNTokens(1))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string id_str = seq.sliceNextToken();

			id_t id = stoi(id_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				sendToClient("Err! Cannot find track by id");
				return EKernelAPIStatus::eErr;
			}

			track->solo = !track->solo;

			sendToClient("Track solo inverted");
			return EKernelAPIStatus::eOk;
			break;
		}

		case eVolume:
		{
			if (!seq.hasNTokens(2))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string volume_str = seq.sliceNextToken();
			uint32_t volume = stoi(volume_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				sendToClient("Err! Cannot find track by id");
				return EKernelAPIStatus::eErr;
			}

			track->level = volume;

			sendToClient("Track volume changed");
			return EKernelAPIStatus::eOk;
		}

		case eGain:
		{
			if (!seq.hasNTokens(2))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string gain_str = seq.sliceNextToken();
			uint32_t gain = stoi(gain_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				sendToClient("Err! Cannot find track by id");
				return EKernelAPIStatus::eErr;
			}

			track->gain = gain;

			sendToClient("Track gain changed");
			return EKernelAPIStatus::eOk;
		}

		case ePan:
		{
			if (!seq.hasNTokens(2))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);


			std::string pan_str = seq.sliceNextToken();
			uint32_t pan = stoi(pan_str);

			Track* track = ItemsManager::getTrack(id);

			if (!track)
			{
				sendToClient("Err");
				return EKernelAPIStatus::eErr;
			}

			track->pan = pan;

			sendToClient("Track pan changed!");
			return EKernelAPIStatus::eOk;
		}

        case eName:
        {
            if (!seq.hasNTokens(2))
            {
                sendToClient("Err: invalid args");
                return EKernelAPIStatus::eErr;
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);


            std::string name_str = seq.sliceNextToken();
            
            Track* track = ItemsManager::getTrack(id);

            if (!track)
            {
                sendToClient("Err");
                return EKernelAPIStatus::eErr;
            }

            track->name = name_str;

            sendToClient("Track name changed!");
            return EKernelAPIStatus::eOk;
        }

		}

        sendToClient("Err! Cannot find such command in 'track' section");
		return EKernelAPIStatus::eErr;
	}
}