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
                return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Fragment* fragment = ItemsManager::getFragment(id);
            if (!fragment)
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
            }

            std::stringstream sstream;
            sstream << "audio: " << fragment->linked_audio << ";";
            sstream << "crop from: " << fragment->crop_from << ";";
            sstream << "crop to: " << fragment->crop_to << ";";
            sstream << "offset: " << fragment->time_offset << ";";

            return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }

        case eList:
        {
            std::set<id_t> fragment_ids = ItemsManager::getFragments();

            std::stringstream sstream;

			sstream << "[";

            for (id_t id : fragment_ids)
            {
                Fragment* fragment = ItemsManager::getFragment(id);

                if (!fragment)
                {
					return APIResponse(EKernelAPIStatus::eErr, sstream.str());
                }

                sstream << "id: " << id;
                sstream << " audio: " << fragment->linked_audio;
                sstream << "; ";
            }

			sstream << "]";

            return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }

        case eAdd:
        {
            if (!seq.hasNTokens(1))
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
            }

            std::string audio_id_str = seq.sliceNextToken();
            id_t audio_id = stoi(audio_id_str);

			if (!ItemsManager::getAudio(audio_id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            id_t id = ItemsManager::createFragment(audio_id);

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

			if (!ItemsManager::getFragment(id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            if (!ItemsManager::removeFragment(id))
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
            }

			return APIResponse(EKernelAPIStatus::eOk);
        }

		case eCrop:
		{
			if (!seq.hasNTokens(3))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);

			Fragment *fragment = ItemsManager::getFragment(id);

			if (!fragment)
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

			{
				std::string crop_from_str = seq.sliceNextToken();
				int32_t crop_from = stoi(crop_from_str);

				std::string crop_to_str = seq.sliceNextToken();
				int32_t crop_to = stoi(crop_to_str);

				if (crop_from < 0 || crop_to < 0 || crop_to <= crop_from)
				{
					return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_arg_value);
				}

				fragment->crop_from = crop_from;
				fragment->crop_to = crop_to;
			}

			return APIResponse(EKernelAPIStatus::eOk);
		}

        case eOffset:
        {
            if (!seq.hasNTokens(2))
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Fragment* fragment = ItemsManager::getFragment(id);

            if (!fragment)
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
            }

            {
                std::string offset_str = seq.sliceNextToken();
                int offset = stoi(offset_str);

				if (offset < 0)
				{
					return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_arg_value);
				}

                fragment->time_offset = offset;
            }

            return APIResponse(EKernelAPIStatus::eOk);
        }

        }

        return APIResponse(EKernelAPIStatus::eErr, c_err_cannot_find_command);
    }
}