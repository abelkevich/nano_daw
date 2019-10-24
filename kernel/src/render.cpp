#include "render.h"
#include "codec_manager.h"
#include "items_manager.h"

uint32_t msToSamples(Session ses, uint32_t ms)
{
    return ses.sample_rate/1000 * ms;
}

static uint32_t calcSessionLength(Session ses)
{
    uint32_t max_audio_len = 0;

	for (auto track_id : ses.tracks)
	{
		Track *track = ItemsManager::getTrack(track_id);

		if (!track)
		{
			continue;
		}

		for (auto fragment_id : track->fragments)
		{
			Fragment *fragment = ItemsManager::getFragment(fragment_id);

			if (!fragment)
			{
				continue;
			}
            uint32_t len = fragment->crop_to - fragment->crop_from;
            uint32_t overall_len = len + fragment->time_offset;

            if (max_audio_len < overall_len)
            {
                max_audio_len = overall_len;
            }
        }
    }

    return max_audio_len;
}

static void mixAudioToOutBuffer(Session ses, Fragment *fragment, float *out_buf)
{
    float* out_shifted_buf = out_buf + msToSamples(ses, fragment->time_offset);

	Audio *audio = ItemsManager::getAudio(fragment->linked_audio);

	if (!audio)
	{
		return;
	}

    float* audio_shifted_buf = audio->buffer + msToSamples(ses, fragment->crop_from);
    uint32_t audio_buf_len = msToSamples(ses, fragment->crop_to);

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
    for (auto track_id: ses.tracks)
    {
		Track *track = ItemsManager::getTrack(track_id);

		if (!track)
		{
			continue;
		}

        for (auto fragment_id: track->fragments)
        {
			Fragment *fragment = ItemsManager::getFragment(fragment_id);

			if (!fragment)
			{
				continue;
			}

            mixAudioToOutBuffer(ses, fragment, left_buf);
            mixAudioToOutBuffer(ses, fragment, right_buf);
        }
    }

    float* arr[2] = {left_buf, right_buf};

    CodecFileInfo file_info(mix_path, arr, ses_len, 1, ses.sample_rate);
    
    codec_info.save_file_proc(file_info, 2);

    delete[] left_buf;
    delete[] right_buf;

    return 0;
}