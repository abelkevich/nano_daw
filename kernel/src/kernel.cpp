#include "kernel.h"
#include "kernel_api.h"
#include "codec_manager.h"
#include "render.h"

#include <sstream>

bool g_working = true;
Session *g_session = nullptr;

namespace Kernel
{
	status_t initKernelCodecs()
	{
		return initCodecs();
	}

	void addTrack(std::string name)
	{
		Track t(name);
		g_session->addTrack(t);
	}

	status_t loadAudioOnTrack(std::string audio_path, id_t track_id)
	{
		Track* track = g_session->getTrack(track_id);

		if (!track)
		{
			return 1;
		}

		CodecInfo codec;

		if (getCodec("pure_wave.dll", codec) != 0)
		{
			return 2;
		}

		CodecFileInfo file_info;
		if (codec.load_file_proc(file_info, audio_path) != 0)
		{
			return 3;
		}

		Audio a(audio_path, file_info.buffers[0], file_info.samples_per_channel);

		track->addAudio(a);

		return 0;
	}

	status_t renderAll(std::string mix_path)
	{
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
