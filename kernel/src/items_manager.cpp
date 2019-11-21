#include "items_manager.h"
#include "codec_manager.h"
#include <map>

namespace ItemsManager
{
	static std::map<id_t, Effect*> g_effects;
	static std::map<id_t, Audio*> g_audios;
	static std::map<id_t, Fragment*> g_fragments;
	static std::map<id_t, Track*> g_tracks;

	id_t genUniqueId()
	{
		static id_t n = 1;
		return n++;
	}

	id_t createEffect(std::string name, std::string params)
	{
		Effect *effect = new Effect(name, params);
		id_t id = genUniqueId();

        LOG_F(INFO, "Created effect with id: '%d' name: '%s' params: '%s'",
                    id, name.c_str(), params.c_str());

		auto rec = std::make_pair(id, effect);
		g_effects.insert(rec);

		return id;
	}

	id_t createAudio(std::string path)
	{
        LOG_F(INFO, "Loading audio from path: %s", path.c_str());

        CodecManager::CodecInfo codec_info;
        if (CodecManager::getCodec("pure_wave.dll", codec_info) != 0)
        {
            LOG_F(ERROR, "Cannot load codec 'pure_wave.dll'");
            return 0;
        }

        CodecFileInfo codec_file_info;
        if (codec_info.load_file_proc(codec_file_info, path) != 0)
        {
            LOG_F(ERROR, "Cannot load file content");
            return 0;
        }

		Audio *audio = new Audio(path, codec_file_info.buffers[0], codec_file_info.samples_per_channel);
		id_t id = genUniqueId();

        LOG_F(INFO, "Created audio with id: '%d' path: '%s'",
                    id, path.c_str());

		auto rec = std::make_pair(id, audio);
		g_audios.insert(rec);

		return id;
	}

	id_t createFragment(id_t linked_audio)
	{
        LOG_F(INFO, "Creating fragment with linked audio %d", linked_audio);

		if (!getAudio(linked_audio))
		{
            LOG_F(ERROR, "Cannot find linked audio");
			return 0;
		}

		Fragment *fragment = new Fragment(linked_audio);
		id_t id = genUniqueId();

        LOG_F(INFO, "Created fragment id: '%d' audio: '%d'", id, linked_audio);

		auto rec = std::make_pair(id, fragment);
		g_fragments.insert(rec);

		return id;
	}

	id_t createTrack(std::string name)
	{
		Track *track = new Track(name);
		id_t id = genUniqueId();

        LOG_F(INFO, "Created track id: '%d' name: '%d'", id, name);

		auto rec = std::make_pair(id, track);
		g_tracks.insert(rec);

		return id;
	}

	Effect* getEffect(id_t id)
	{
		if (g_effects.find(id) == g_effects.end())
		{
			return nullptr;
		}

		return g_effects.at(id);
	}

	Audio* getAudio(id_t id)
	{
		if (g_audios.find(id) == g_audios.end())
		{
			return nullptr;
		}

		return g_audios.at(id);
	}

	Fragment* getFragment(id_t id)
	{
		if (g_fragments.find(id) == g_fragments.end())
		{
			return nullptr;
		}

		return g_fragments.at(id);
	}

	Track* getTrack(id_t id)
	{
		if (g_tracks.find(id) == g_tracks.end())
		{
			return nullptr;
		}

		return g_tracks.at(id);
	}

	bool removeEffect(id_t id)
	{
        LOG_F(INFO, "Removing track id: '%d'", id);

		if (g_effects.find(id) == g_effects.end())
		{
            LOG_F(ERROR, "Cannot find track id");
			return false;
		}

		delete g_effects.at(id);
		g_effects.erase(id);

        LOG_F(INFO, "Track removed");

		return true;
	}

	bool removeAudio(id_t id)
	{
        LOG_F(INFO, "Removing audio id: '%d'", id);

		if (g_audios.find(id) == g_audios.end())
		{
            LOG_F(ERROR, "Cannot find audio id");
			return false;
		}

		delete g_audios.at(id);
		g_audios.erase(id);

        LOG_F(INFO, "Track removed");

		return true;
	}

	bool removeFragment(id_t id)
	{
        LOG_F(INFO, "Removing fragment id: '%d'", id);

		if (g_fragments.find(id) == g_fragments.end())
		{
            LOG_F(ERROR, "Cannot find fragment id");
			return false;
		}

		delete g_fragments.at(id);
		g_fragments.erase(id);

        LOG_F(INFO, "Fragment removed");

		return true;
	}

	bool removeTrack(id_t id)
	{
        LOG_F(INFO, "Removing track id: '%d'", id);

		if (g_tracks.find(id) == g_tracks.end())
		{
            LOG_F(ERROR, "Cannot find track id");
			return false;
		}

		delete g_tracks.at(id);
		g_tracks.erase(id);

        LOG_F(INFO, "Track removed");

		return true;
	}

	std::set<id_t> getEffects()
	{
		std::set<id_t> set;

		for (auto e : g_effects)
		{
			set.insert(e.first);
		}

		return set;
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

	std::set<id_t> getFragments()
	{
		std::set<id_t> set;

		for (auto f : g_fragments)
		{
			set.insert(f.first);
		}

		return set;
	}

	std::set<id_t> getTracks()
	{
		std::set<id_t> set;

		for (auto t : g_tracks)
		{
			set.insert(t.first);
		}

		return set;
	}
}