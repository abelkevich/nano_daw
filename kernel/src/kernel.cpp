#include "kernel.h"
#include "kernel_api.h"
#include "codec_manager.h"
#include "render.h"

#include <sstream>

bool g_working = true;
Session *g_session = nullptr;

namespace Kernel
{
    status_t createSession(std::string name, std::string path, uint32_t sample_rate)
    {
        if (g_session) { return 1; }

        g_session = new Session(name, path, sample_rate);
        return 0;
    }

	status_t initKernelCodecs()
	{
		return initCodecs();
	}

	status_t addTrack(std::string name)
	{
        if (!g_session) { return 1; }

        return g_session->addTrack(Track(name));
	}

	status_t loadAudioOnTrack(std::string audio_path, id_t track_id)
	{
		if (!g_session) { return 1; }

		Track* track = g_session->getTrack(track_id);
		if (!track) { return 1; }

		CodecInfo codec;
		if (getCodec("pure_wave.dll", codec) != 0) { return 2; }

		CodecFileInfo file_info;
		if (codec.load_file_proc(file_info, audio_path) != 0) { return 3; }

		Audio a(audio_path, file_info.buffers[0], file_info.samples_per_channel);

		track->addAudio(a);
		return 0;
	}

	status_t renderAll(std::string mix_path)
	{
		if (!g_session) { return 1; }

		return render(*g_session, mix_path);
	}

	std::string listInitedCodecs()
	{
		std::vector<CodecInfo> codecs = getInitedCodecs();

		std::stringstream str_stream;
		str_stream << "Codecs available:\n";

		for (auto codec : codecs)
		{
			str_stream << "\t" << codec.lib_name << std::endl;
		}

		return str_stream.str();
	}

    std::string listTracks()
    {
		if (!g_session) { return std::string(); }

        std::stringstream str_stream;
        str_stream << "Tracks available:\n";

        for (auto track : g_session->tracks)
        {
            str_stream << "\tid:" << track.first;
            str_stream << " name: " << track.second.name;
            str_stream << std::endl;
        }

        return str_stream.str();
    }

	status_t removeTrack(id_t id)
	{
		if (!g_session) { return 1; }

		return g_session->removeTrack(id);
	}

	status_t muteTrack(id_t id)
	{
		if (!g_session) { return 1; }

		Track* track = g_session->getTrack(id);
		if (!track) { return 2; }

		track->mute = !track->mute;
		return 0;
	}

	status_t soloTrack(id_t id)
	{
		if (!g_session) { return 1; }

		Track* track = g_session->getTrack(id);
		if (!track) { return 2; }

		track->solo = !track->solo;
		return 0;
	}

	status_t volumeTrack(id_t id, uint32_t volume)
	{
		if (!g_session) { return 1; }

		Track* track = g_session->getTrack(id);
		if (!track) { return 2; }

		track->level = volume;
		return 0;
	}

	status_t panTrack(id_t id, uint32_t pan)
	{
		if (!g_session) { return 1; }

		Track* track = g_session->getTrack(id);
		if (!track) { return 2; }

		track->pan = pan;
		return 0;
	}

	status_t gainTrack(id_t id, uint32_t gain)
	{
		if (!g_session) { return 1; }

		Track* track = g_session->getTrack(id);
		if (!track) { return 2; }

		track->gain = gain;
		return 0;
	}
}

int main(int argc, char **argv)
{
	initKernelAPI();

    while (g_working)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  
    return 0;
}
