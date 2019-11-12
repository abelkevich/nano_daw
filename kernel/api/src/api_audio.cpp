#include "api_render.h"
#include "items_manager.h"

namespace ClientAPI
{
    json cmdAudio(CommandSeq seq)
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
                return json({{"err_msg", c_err_invalid_args_number}});
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Audio* audio = ItemsManager::getAudio(id);
            if (!audio)
            {
                return json({ {"err_msg", c_err_invalid_id} });
            }

            return json({ {"size: ", audio->getBufferLength()} });
        }

        case eList:
        {

            std::set<id_t> audio_ids = ItemsManager::getAudios();
            
            json::array_t response = json::array();

            for (id_t id : audio_ids)
            {
                Audio* audio = ItemsManager::getAudio(id);

                if (!audio)
                {
                    return json({ {"err_msg", c_err_operation_failed} });
                }

                
                json arr_elem({ {"id", id}, {"path", audio->getPath()} });
                response.push_back(arr_elem);
            }

			
            return response;
        }

        case eAdd:
        {
            if (!seq.hasNTokens(1))
            {
                return json({ {"err_msg", c_err_invalid_args_number} });
            }

            std::string path = seq.sliceNextToken();

            id_t id = ItemsManager::createAudio(path);

            if (!id)
            {
                return json({ {"err_msg", c_err_operation_failed} });
            }

            return json({ {"id", id} });
        }

        case eRemove:
        {
            if (!seq.hasNTokens(1))
            {
                return json({ {"err_msg", c_err_invalid_args_number} });
            }

            std::string id_str = seq.sliceNextToken();

            id_t id = stoi(id_str);

			if (!ItemsManager::getAudio(id))
			{
                return json({ {"err_msg", c_err_invalid_id} });
			}

            if (!ItemsManager::removeAudio(id))
            {
                return json({ {"err_msg", c_err_operation_failed} });
            }

            return json({});
        }

        }

        return json({ {"err_msg", c_err_cannot_find_command} });
    }
}