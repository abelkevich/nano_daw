#include "render.h"
#include "codec_manager.h"

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
    CodecInfo codec_info;
    
    if (getCodec("pure_wave.dll", codec_info) != 0)
    {
        return 1;
    }

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

    float* arr[2] = {left_buf, right_buf};

    CodecFileInfo file_info(mix_path, arr, ses_len, 1, ses.sample_rate);
    
    codec_info.save_file_proc(file_info, 2);

    delete[] left_buf;
    delete[] right_buf;

    return 0;
}