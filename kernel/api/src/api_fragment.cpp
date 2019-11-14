#include "api_render.h"
#include "items_manager.h"

namespace ClientAPI
{
    json cmdFragment(CommandSeq seq)
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
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Fragment* fragment = ItemsManager::getFragment(id);
            if (!fragment)
            {
                return jsonErrResponse(EErrCodes::eInvalidFragment);
            }

            return json({ {"audio", fragment->getAudio()}, {"crop_from", fragment->getCropFrom()}, {"crop_to", fragment->getCropTo()}
                        , {"offset", fragment->getTimeOffset()}});
        }

        case eList:
        {
            std::set<id_t> fragment_ids = ItemsManager::getFragments();

            json::array_t response = json::array();
            for (id_t id : fragment_ids)
            {
                Fragment* fragment = ItemsManager::getFragment(id);

                if (!fragment)
                {
                    return jsonErrResponse(EErrCodes::eOperationFailed);
                }

                json arr_elem({ {"id", id}, {"audio", fragment->getAudio()} });
                response.push_back(arr_elem);
            }

            return response;
        }

        case eAdd:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string audio_id_str = seq.sliceNextToken();
            id_t audio_id = stoi(audio_id_str);

			if (!ItemsManager::getAudio(audio_id))
			{
                return jsonErrResponse(EErrCodes::eInvalidAudio);
			}

            id_t id = ItemsManager::createFragment(audio_id);

			if (!id)
			{
                return jsonErrResponse(EErrCodes::eOperationFailed);
			}

            return json({ {"id", id} });
        }

        case eRemove:
        {
            if (!seq.hasNTokens(1))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();

            id_t id = stoi(id_str);

			if (!ItemsManager::getFragment(id))
			{
                return jsonErrResponse(EErrCodes::eInvalidFragment);
			}

            if (!ItemsManager::removeFragment(id))
            {
                return jsonErrResponse(EErrCodes::eOperationFailed);
            }

            return json({ {"id", id} });
        }

		case eCrop:
		{
			if (!seq.hasNTokens(3))
			{
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
			}

			std::string id_str = seq.sliceNextToken();
			id_t id = stoi(id_str);

			Fragment *fragment = ItemsManager::getFragment(id);

			if (!fragment)
			{
                return jsonErrResponse(EErrCodes::eInvalidFragment);
			}

			{
				std::string crop_from_str = seq.sliceNextToken();
				int32_t crop_from = stoi(crop_from_str);

				std::string crop_to_str = seq.sliceNextToken();
				int32_t crop_to = stoi(crop_to_str);

				if (crop_from < 0 || crop_to < 0 || crop_to <= crop_from)
				{
                    return jsonErrResponse(EErrCodes::eInvalidArgValue);
				}

				fragment->crop(crop_from, crop_to);
			}

            return json({ {"id", id} });
		}

        case eOffset:
        {
            if (!seq.hasNTokens(2))
            {
                return jsonErrResponse(EErrCodes::eInvalidArgsNum);
            }

            std::string id_str = seq.sliceNextToken();
            id_t id = stoi(id_str);

            Fragment* fragment = ItemsManager::getFragment(id);

            if (!fragment)
            {
                return jsonErrResponse(EErrCodes::eInvalidFragment);
            }

            {
                std::string offset_str = seq.sliceNextToken();
                int offset = stoi(offset_str);

				if (offset < 0)
				{
                    return jsonErrResponse(EErrCodes::eInvalidArgValue);
				}

                fragment->setTimeOffset(offset);
            }

            return json({ {"id", id} });
        }

        }

        return jsonErrResponse(EErrCodes::eCommandNotFound);
    }
}