#include "session.h"
#include "pure_wave.h"

uint32_t loadWavFile(std::string path, Codecs::PureWave::level_t** file_buffer)
{
    using namespace Codecs::PureWave;

    WavHeader wav_header;

    FILE* f = fopen(path.c_str(), "rb");
    readHeader(f, wav_header);
    allocBuffer(wav_header, file_buffer);
    readData(f, wav_header, *file_buffer);
    fclose(f);

    return wav_header.data_bytes / 2; // buf len in samples
}

Session genDummySession()
{
    using namespace Codecs::PureWave;

    auto loadAudio = [](std::string path) -> Audio
    {
        level_t* file_buffer_fixed = nullptr;
        uint32_t file_samples_len = loadWavFile(path, &file_buffer_fixed);

        float* file_buffer_float = new float[file_samples_len];

        for (uint32_t i = 0; i < file_samples_len; i++)
        {
            level_t val = file_buffer_fixed[i];

            file_buffer_float[i] = (float) val / INT16_MAX;
        }

        freeBuffer(file_buffer_fixed);

        return Audio(path, 0, 0, 20000, file_buffer_float, file_samples_len);
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