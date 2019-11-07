#include "api_render.h"
#include "items_manager.h"

namespace ClientAPI
{
    APIResponse cmdFragment(CommandSeq seq)
    {
        enum EIdents { eList, eAdd, eRemove, eCrop, eOffset, eInfo, eNone };
		IdentsMap<EIdents> idents_map{ {"add", eAdd}, {"remove", eRemove}, {"list", eList}, 
                                       {"crop", eCrop}, {"offset", eOffset}, {"info", eInfo} };

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

            Fragment* fragment = ItemsManager::getFragment(id);
            if (!fragment)
            {
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find fragment by id");
            }

            std::stringstream sstream;
            sstream << "linked: " << fragment->linked_audio << ";";
            sstream << "crop from: " << fragment->crop_from << ";";
            sstream << "crop to: " << fragment->crop_to << ";";
            sstream << "offset: " << fragment->time_offset << ";";

            return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }

        case eList:
        {
            std::set<id_t> fragment_ids = ItemsManager::getFragments();

            std::stringstream sstream;

            for (id_t id : fragment_ids)
            {
                Fragment* fragment = ItemsManager::getFragment(id);

                if (!fragment)
                {
					return APIResponse(EKernelAPIStatus::eErr, sstream.str());
                }

                sstream << "Fragment: id: " << id;
                sstream << " linked audio: " << fragment->linked_audio;
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

            std::string audio_id_str = seq.sliceNextToken();

            id_t audio_id = stoi(audio_id_str);
            id_t id = ItemsManager::createFragment(audio_id);

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

            if (!ItemsManager::removeFragment(id))
            {
				return APIResponse(EKernelAPIStatus::eErr);
            }

			return APIResponse(EKernelAPIStatus::eOk);
        }

		case eCrop:
		{
			if (!seq.hasNTokens(3))
			{
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);

			Fragment *fragment = ItemsManager::getFragment(id);

			if (!fragment)
			{
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
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

			return APIResponse(EKernelAPIStatus::eOk);
		}

        case eOffset:
        {
            if (!seq.hasNTokens(2))
            {
				return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Fragment* fragment = ItemsManager::getFragment(id);

            if (!fragment)
            {
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
            }

            {
                std::string offset_str = seq.sliceNextToken();
                uint32_t offset = stoi(offset_str);
                fragment->time_offset = offset;
            }

            return APIResponse(EKernelAPIStatus::eOk);
        }

        }

        return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command in 'fragment' section");
    }
}