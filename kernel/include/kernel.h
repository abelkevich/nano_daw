#pragma once

#include "common.h"
#include "session.h"

extern bool g_working;
extern Session *g_session;

namespace Kernel
{
    status_t createSession(std::string name, std::string path, uint32_t sample_rate);
	status_t initKernelCodecs();
	std::string listInitedCodecs();
	status_t renderAll(std::string mix_path);
	status_t loadAudioOnTrack(std::string audio_path, id_t track_id);
	status_t addTrack(std::string name);
    std::string listTracks();
}