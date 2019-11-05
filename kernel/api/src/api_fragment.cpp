#include "api_render.h"
#include "items_manager.h"

namespace ClientAPI
{
    EKernelAPIStatus cmdFragment(CommandSeq seq)
    {
        enum EIdents { eList, eAdd, eRemove, eCrop, eNone };
		IdentsMap<EIdents> idents_map{ {"add", eAdd}, {"remove", eRemove}, {"list", eList}, {"crop", eCrop} };

        std::string token = seq.sliceNextToken();

        EIdents cmd = idents_map.hasIdent(token) ?
            idents_map.getIdent(token) : eNone;

        switch (cmd)
        {
        case eList:
        {
            std::set<id_t> fragment_ids = ItemsManager::getFragments();

            std::stringstream sstream;

            for (id_t id : fragment_ids)
            {
                Fragment* fragment = ItemsManager::getFragment(id);

                if (!fragment)
                {
                    continue;
                }

                sstream << "Fragment: id: " << id;
                sstream << " linked audio: " << fragment->linked_audio;
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

            std::string audio_id_str = seq.sliceNextToken();

            id_t audio_id = stoi(audio_id_str);
            id_t id = ItemsManager::createFragment(audio_id);

			if (!id)
			{
				sendToClient("Err! Cannot create fragment");
				return EKernelAPIStatus::eErr;
			}

            sendToClient("Fragment added! id: " + std::to_string(id));
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

            if (!ItemsManager::removeFragment(id))
            {
                sendToClient("Err");
                return EKernelAPIStatus::eErr;
            }

            sendToClient("Fragment removed!");
            return EKernelAPIStatus::eOk;
        }

		case eCrop:
		{
			if (!seq.hasNTokens(3))
			{
				sendToClient("Err: invalid args");
				return EKernelAPIStatus::eErr;
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);

			Fragment *fragment = ItemsManager::getFragment(id);

			if (!fragment)
			{
				sendToClient("Err! Cannot find fragment by id");
				return EKernelAPIStatus::eErr;
			}

			{
				std::string crop_from_str = seq.sliceNextToken();
				uint32_t crop_from = stoi(crop_from_str);
				fragment->crop_from = crop_from;
			}

			{
				std::string crop_to_str = seq.sliceNextToken();
				uint32_t crop_to = stoi(crop_to_str);
				fragment->crop_to = crop_to;
			}

			sendToClient("Fragment cropped!");
			return EKernelAPIStatus::eOk;
		}

        }

        sendToClient("Err! Cannot find such command in 'fragment' section");
        return EKernelAPIStatus::eErr;
    }
}