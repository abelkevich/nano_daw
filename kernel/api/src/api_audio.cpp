#include "api_render.h"
#include "items_manager.h"

namespace ClientAPI
{
    APIResponse cmdAudio(CommandSeq seq)
    {
        enum EIdents { eList, eAdd, eRemove, eInfo, eNone };
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
                return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_args_number);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Audio* audio = ItemsManager::getAudio(id);
            if (!audio)
            {
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
            }

            std::stringstream sstream;
            sstream << "size: " << audio->getBufferLength() << ";";

            return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }

        case eList:
        {

            std::set<id_t> audio_ids = ItemsManager::getAudios();

            std::stringstream sstream;

			sstream << "[";

            for (id_t id : audio_ids)
            {
                Audio* audio = ItemsManager::getAudio(id);

                if (!audio)
                {
					return APIResponse(EKernelAPIStatus::eErr, sstream.str());
                }

                sstream << "id: " << id;
                sstream << " path: " << audio->getPath();
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

            std::string path = seq.sliceNextToken();

            id_t id = ItemsManager::createAudio(path);

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

			if (!ItemsManager::getAudio(id))
			{
				return APIResponse(EKernelAPIStatus::eErr, c_err_invalid_id);
			}

            if (!ItemsManager::removeAudio(id))
            {
                return APIResponse(EKernelAPIStatus::eErr, c_err_operation_failed);
            }

            return APIResponse(EKernelAPIStatus::eOk);
        }

        }

        return APIResponse(EKernelAPIStatus::eErr, c_err_cannot_find_command);
    }
}