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
                return APIResponse(EKernelAPIStatus::eErr, "Invalid args");
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Audio* audio = ItemsManager::getAudio(id);
            if (!audio)
            {
				return APIResponse(EKernelAPIStatus::eErr, "Cannot find id");
            }

            std::stringstream sstream;
            sstream << "size: " << audio->buffer_size << ";";

            return APIResponse(EKernelAPIStatus::eOk, sstream.str());
        }

        case eList:
        {

            std::set<id_t> audio_ids = ItemsManager::getAudios();

            std::stringstream sstream;

            for (id_t id : audio_ids)
            {
                Audio* audio = ItemsManager::getAudio(id);

                if (!audio)
                {
					return APIResponse(EKernelAPIStatus::eErr, sstream.str());
                }

                sstream << "Audio: id: " << id;
                sstream << " path: " << audio->path;
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

            std::string path = seq.sliceNextToken();

            id_t id = ItemsManager::createAudio(path);

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

            if (!ItemsManager::removeAudio(id))
            {
                return APIResponse(EKernelAPIStatus::eErr);
            }

            return APIResponse(EKernelAPIStatus::eOk);
        }

        }

        return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command in 'audio' section");
    }
}