#pragma once

#include "common.h"
#include "session.h"

extern bool g_working;
extern Session *g_session;

namespace Kernel
{
	status_t initKernelCodecs();
	std::string listInitedCodecs();
	status_t renderAll(std::string mix_path);
	status_t loadAudioOnTrack(std::string audio_path, id_t track_id);
	void addTrack(std::string name);
}