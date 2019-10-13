#include "render.h"
#include "pure_wave.h"

uint32_t msToSamples(Session ses, uint32_t ms)
{
    return ses.sample_rate/1000 * ms;
}

static uint32_t calcSessionLength(Session ses)
{
    uint32_t max_audio_len = 0;

    for (auto t: ses.tracks)
    {
        for (auto a : t.audio)
        {
            uint32_t len = a.crop_to - a.crop_from;
            uint32_t overall_len = len + a.time_offset;

            if (max_audio_len < overall_len)
            {
                max_audio_len = overall_len;
            }
        }
    }

    return max_audio_len;
}

static void mixAudioToOutBuffer(Session ses, Audio audio, float *out_buf)
{
    float* out_shifted_buf = out_buf + msToSamples(ses, audio.time_offset);

    float* audio_shifted_buf = audio.buffer + msToSamples(ses, audio.crop_from);
    uint32_t audio_buf_len = msToSamples(ses, audio.crop_to);

    for (uint32_t i = 0; i < audio_buf_len; i++)
    {
        out_shifted_buf[i] += audio_shifted_buf[i];
    }
}

status_t render(Session ses, std::string mix_path)
{
    using namespace Codecs::PureWave;

    // calc session length in samples
    uint32_t ses_len = msToSamples(ses, calcSessionLength(ses));
    
    // allocate intermediate buffs
    float* left_buf = new float[ses_len];
    float* right_buf = new float[ses_len];

    for (uint32_t i = 0; i < ses_len; i++)
    {
        left_buf[i] = 0;
        right_buf[i] = 0;
    }

    // mix right and left channels
    for (auto t: ses.tracks)
    {
        for (auto a: t.audio)
        {
            mixAudioToOutBuffer(ses, a, left_buf);
            mixAudioToOutBuffer(ses, a, right_buf);
        }
    }

    // Create and fill stereo 16-bit mix buffer
    level_t* mix_buf = new level_t[ses_len * 2];
    for (uint32_t i = 0; i < ses_len; i++)
    {
        mix_buf[i * 2] = left_buf[i] * INT16_MAX;
        mix_buf[i * 2 + 1] = right_buf[i] * INT16_MAX;
    }
    
    // free intermediate buffs
    delete[] left_buf;
    delete[] right_buf;

    // save mix buffer to file
    uint32_t mix_buf_size = ses_len * sizeof(level_t);
    WavHeader mix_header = makeDefaultHeader(ses.sample_rate, mix_buf_size);

    FILE* file_mix = fopen(mix_path.c_str(), "wb");

    writeHeader(file_mix, mix_header);
    writeData(file_mix, mix_header, mix_buf);

    fclose(file_mix);

    // free mix buf
    delete[] mix_buf;

    return 0;
}