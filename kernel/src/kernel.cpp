#include "kernel.h"
#include "kernel_api.h"
#include "codec_manager.h"

bool g_working = true;
Session g_session;

void addTrack(std::string name)
{
    Track t(name, false, false, 0, 0, 0, {}, {});
    g_session.tracks.push_back(t);
}

status_t loadAudioOnTrack(std::string audio_path, std::string track_name)
{
    Track* track = g_session.getTrack(track_name);
    
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

    Audio a(audio_path, 0, 0, 20000, file_info.buffers[0], file_info.samples_per_channel);
    
    track->audio.push_back(a);

    return 0;
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
