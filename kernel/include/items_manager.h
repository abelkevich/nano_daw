#pragma once
#include "session.h"

namespace ItemsManager
{
	id_t createEffect(std::string name, std::string params);
	id_t createAudio(std::string path, float* buffer, uint32_t buffer_size);
	id_t createFragment(id_t linked_audio);
	id_t createTrack(std::string name);

	Effect* getEffect(id_t id);
	Audio* getAudio(id_t id);
	Fragment* getFragment(id_t id);
	Track* getTrack(id_t id);

	std::set<id_t> getEffects();
	std::set<id_t> getAudios();
	std::set<id_t> getFragments();
	std::set<id_t> getTracks();

	bool removeEffect(id_t id);
	bool removeAudio(id_t id);
	bool removeFragment(id_t id);
	bool removeTrack(id_t id);
}