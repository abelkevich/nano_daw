#include "audios_manager.h"
#include "fragments_manager.h"
#include "codec_manager.h"

namespace AudiosManager
{
    static std::map<id_t, Audio*> g_audios;
    
    id_t createAudio(const std::string& path)
    {
        LOG_F(INFO, "Creating new audio with path: %s", path.c_str());

        const std::string file_ext = Utils::getFileExt(path);
        const CodecManager::Codec* codec = CodecManager::findCodecByFileExt(file_ext);

        if (!codec)
        {
            LOG_F(ERROR, "Cannot find codec to process file with '%s' extension", file_ext.c_str());
            return 0;
        }

        CodecFileInfo codec_file_info;
        if (codec->loadFile(codec_file_info, path) != 0)
        {
            LOG_F(ERROR, "Cannot load file content");
            return 0;
        }

        LOG_F(INFO, "Info processed by codec: channels: '%d'; samples per channel: '%d'; sample rate: '%d'",
                    codec_file_info.channel_num,
                    codec_file_info.samples_per_channel,
                    codec_file_info.sample_rate);

        Audio* audio = new Audio(ItemsManager::genUniqueId(), path, codec_file_info.buffers[0], 
                                 codec_file_info.samples_per_channel, codec_file_info.sample_rate);

        LOG_F(INFO, "Audio (id: '%d') was created", audio->getId());

        g_audios.insert(std::make_pair(audio->getId(), audio));

        return audio->getId();
    }

    bool removeAudio(id_t id)
    {
        auto RemoveFragments = [](Audio* audio) -> void
        {
            LOG_F(INFO, "Removing audio (id: '%d') linked fragments", audio->getId());
            std::set<id_t> linked_fragments_id = audio->getLinkedFragments();

            for (id_t fragment_id : linked_fragments_id)
            {
                if (!FragmentsManager::removeFragment(fragment_id))
                {
                    LOG_F(ERROR, "Cannot remove fragment (id: '%d') from audio (id: '%d'), skipping", fragment_id, audio->getId());
                    continue;
                }
            }
        };

        LOG_F(INFO, "Removing audio (id: '%d')", id);

        Audio* audio = getAudio(id);

        if (!audio)
        {
            LOG_F(ERROR, "Cannot get audio (id: '%d')", id);
            return false;
        }

        RemoveFragments(audio);

        delete g_audios.at(id);
        g_audios.erase(id);

        LOG_F(INFO, "Audio (id: '%d') was removed", id);

        return true;
    }

    Audio* getAudio(id_t id)
    {
        if (g_audios.find(id) == g_audios.end())
        {
            return nullptr;
        }

        return g_audios.at(id);
    }

    std::set<id_t> getAudios()
    {
        std::set<id_t> set;

        for (auto a : g_audios)
        {
            set.insert(a.first);
        }

        return set;
    }
}