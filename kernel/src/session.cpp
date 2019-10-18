#include "session.h"
#include "codec_manager.h"

Session genDummySession()
{
    CodecInfo codec;

    if (getCodec("pure_wave.dll", codec) != 0)
    {
        return Session("none", "none", 0, {});
    }

    auto loadAudio = [=](std::string path) -> Audio
    {
        CodecFileInfo file_info;

        if (codec.load_file_proc(file_info, path))
        {
        }
        
        return Audio(path, 0, 0, 20000, file_info.buffers[0], file_info.samples_per_channel);
    };

    Audio audio_1 = loadAudio("res\\samples_16bit_48khz\\bass_di.wav");
    Audio audio_2 = loadAudio("res\\samples_16bit_48khz\\drums_overhead_l.wav");
    Audio audio_3 = loadAudio("res\\samples_16bit_48khz\\drums_overhead_r.wav");
    Audio audio_4 = loadAudio("res\\samples_16bit_48khz\\drums_kick.wav");
    Audio audio_5 = loadAudio("res\\samples_16bit_48khz\\sax.wav");

    Effect effect_1("echo", "100ms");

    Track track_1(false, false, 0, 1, 0, {audio_1, audio_2}, {});
    Track track_2(false, false, 0, 1, 0, {audio_3, audio_4, audio_5}, {effect_1});

    Session session("sample", "sample.proj", 48000, {track_1, track_2});

    return session;
}