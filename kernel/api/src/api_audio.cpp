#include "api_render.h"
#include "items_manager.h"

namespace ClientAPI
{
    EKernelAPIStatus cmdAudio(CommandSeq seq)
    {
        enum EIdents { eList, eAdd, eRemove, eNone };
        IdentsMap<EIdents> idents_map{ {"add", eAdd}, {"remove", eRemove}, {"list", eList} };

        std::string token = seq.sliceNextToken();

        EIdents cmd = idents_map.hasIdent(token) ?
            idents_map.getIdent(token) : eNone;

        switch (cmd)
        {
        case eList:
        {

            std::set<id_t> audio_ids = ItemsManager::getAudios();

            std::stringstream sstream;

            for (id_t id : audio_ids)
            {
                Audio* audio = ItemsManager::getAudio(id);

                if (!audio)
                {
                    continue;
                }

                sstream << "Audio: id: " << id;
                sstream << " path: " << audio->path;
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

            std::string path = seq.sliceNextToken();

            id_t id = ItemsManager::createAudio(path);

            if (!id)
            {
                sendToClient("Err");
                return EKernelAPIStatus::eErr;
            }

            sendToClient("Audio added! id: " + std::to_string(id));
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

            if (!ItemsManager::removeAudio(id))
            {
                sendToClient("Err");
                return EKernelAPIStatus::eErr;
            }

            sendToClient("Audio removed!");
            return EKernelAPIStatus::eOk;
        }

        }

        return EKernelAPIStatus::eErr;
    }
}