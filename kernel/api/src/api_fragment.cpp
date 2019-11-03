#include "api_render.h"
#include "items_manager.h"

namespace ClientAPI
{
    EKernelAPIStatus cmdFragment(CommandSeq seq)
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

        }

        return EKernelAPIStatus::eErr;
    }
}