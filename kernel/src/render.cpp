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

	for (auto track_id: ses.tracks)
	{
		Track *track = ItemsManager::getTrack(track_id);

		if (!track)
		{
			return 0;
		}

		for (auto fragment_id : track->fragments)
		{
			Fragment *fragment = ItemsManager::getFragment(fragment_id);

			if (!fragment)
			{
				return 0;
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

static status_t mixAudioToOutBuffer(Session ses, Fragment *fragment, float *out_buf)
{
	Audio *audio = ItemsManager::getAudio(fragment->linked_audio);

	if (!audio)
	{
		return 1;
	}

    uint32_t audio_from_smp = msToSamples(ses, fragment->crop_from);
    uint32_t audio_to_smp = msToSamples(ses, fragment->crop_to);
	uint32_t audio_len_smp = audio_to_smp - audio_from_smp;
	uint32_t out_buf_from_smp = msToSamples(ses, fragment->time_offset);

	if (audio_from_smp >= audio_to_smp)
	{
		return 2;
	}

	if (out_buf_from_smp + audio_len_smp > msToSamples(ses, calcSessionLength(ses)))
	{
		return 3;
	}

    for (uint32_t i = 0; i < audio_len_smp; i++)
    {
        out_buf[out_buf_from_smp + i] += audio->buffer[audio_from_smp + i];
    }

	return 0;
}

status_t render(Session ses, std::string mix_path)
{
    // calc session length in samples
    uint32_t ses_len_smp = msToSamples(ses, calcSessionLength(ses));
    
    // allocate intermediate buffs
    float* left_buf = new float[ses_len_smp];
    float* right_buf = new float[ses_len_smp];

    for (uint32_t i = 0; i < ses_len_smp; i++)
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
			return 2;
		}

        for (auto fragment_id: track->fragments)
        {
			Fragment *fragment = ItemsManager::getFragment(fragment_id);

			if (!fragment)
			{
				return 3;
			}

			if (mixAudioToOutBuffer(ses, fragment, left_buf) != 0 ||
				mixAudioToOutBuffer(ses, fragment, right_buf) != 0)
			{
				return 4;
			}
        }
    }


	CodecManager::CodecInfo codec_info;

	if (CodecManager::getCodec("pure_wave.dll", codec_info) != 0)
	{
		return 1;
	}

	float* arr[2] = { left_buf, right_buf };
    CodecFileInfo file_info(mix_path, arr, ses_len_smp, 1, ses.sample_rate);
    
    codec_info.save_file_proc(file_info, 2);

    delete[] left_buf;
    delete[] right_buf;

    return 0;
}