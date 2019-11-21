#include "render.h"
#include "codec_manager.h"
#include "items_manager.h"

static uint32_t calcSessionLength()
{
    uint32_t max_audio_len = 0;

	for (auto track_id: g_session->getTracks())
	{
		Track *track = ItemsManager::getTrack(track_id);

		if (!track)
		{
			return 0;
		}

		for (auto fragment_id : track->getFragments())
		{
			Fragment *fragment = ItemsManager::getFragment(fragment_id);

			if (!fragment)
			{
				return 0;
			}

            uint32_t len = fragment->getCropTo() - fragment->getCropFrom();
            uint32_t overall_len = len + fragment->getTimeOffset();

            if (max_audio_len < overall_len)
            {
                max_audio_len = overall_len;
            }
        }
    }

    return max_audio_len;
}

static status_t mixAudioToOutBuffer(Fragment *fragment, float *out_buf, uint8_t gain, uint8_t level)
{
	Audio *audio = ItemsManager::getAudio(fragment->getAudio());

	if (!audio)
	{
		return 1;
	}

    uint32_t audio_from_smp = g_session->msToSamples(fragment->getCropFrom());
    uint32_t audio_to_smp = g_session->msToSamples(fragment->getCropTo());
	uint32_t audio_len_smp = audio_to_smp - audio_from_smp;
	uint32_t out_buf_from_smp = g_session->msToSamples(fragment->getTimeOffset());

	if (audio_from_smp >= audio_to_smp)
	{
		return 2;
	}

	if (out_buf_from_smp + audio_len_smp > g_session->msToSamples(calcSessionLength()))
	{
		return 3;
	}

    float k = (gain / 10.0 + 1.0) * (level / 100.0);

    for (uint32_t i = 0; i < audio_len_smp; i++)
    {
        out_buf[out_buf_from_smp + i] += audio->getBuffer()[audio_from_smp + i] * k;
    }

	return 0;
}

static std::set<id_t> getTracksWithSolo()
{
    std::set<id_t> solo_tracks_id;

    for (auto track_id : g_session->getTracks())
    {
        Track* track = ItemsManager::getTrack(track_id);

        if (!track)
        {
            return std::set<id_t>();
        }

        if (track->getSolo())
        {
            solo_tracks_id.insert(track_id);
        }
    }

    return solo_tracks_id;
}

status_t render(std::string mix_path)
{

    LOG_F(INFO, "Starting rendering session");
    if (!g_session)
    {

        LOG_F(ERROR, "No session loaded");
        return 1;
    }

    // calc session length in samples
    uint32_t ses_len_ms = calcSessionLength();
    uint32_t ses_len_smp = g_session->msToSamples(ses_len_ms);

    LOG_F(INFO, "Total sessiong length in ms: %d", ses_len_ms);

    LOG_F(INFO, "Allocating intermediate buffers and zeroing them");
    
    // allocate intermediate buffs
    float* left_buf = new float[ses_len_smp];
    float* right_buf = new float[ses_len_smp];

    for (uint32_t i = 0; i < ses_len_smp; i++)
    {
        left_buf[i] = 0;
        right_buf[i] = 0;
    }

    std::set<id_t> solo_tracks = getTracksWithSolo();

    std::set<id_t> tracks_to_mix = !solo_tracks.empty() ? solo_tracks : g_session->getTracks();

    LOG_F(INFO, solo_tracks.empty() ? "There are some solo tracks, mixing only them" :
                                      "Mixing all available tracks");

    // mix right and left channels
    for (auto track_id: tracks_to_mix)
    {
		Track *track = ItemsManager::getTrack(track_id);

		if (!track)
		{
            LOG_F(ERROR, "Failed to get track with id %d", track_id);
			return 2;
		}

        LOG_F(INFO, "Got track %d: %s in mix", track_id, track->getName().c_str());

        if (track->getMute())
        {
            LOG_F(INFO, "Track is muted, skipping");
            continue;
        }

        for (auto fragment_id: track->getFragments())
        {
			Fragment *fragment = ItemsManager::getFragment(fragment_id);

			if (!fragment)
			{
                LOG_F(ERROR, "Failed to get fragment with id %d", fragment_id);
				return 3;
			}

            LOG_F(INFO, "Got fragment %d with linked audio %d", fragment_id, fragment->getAudio());
            LOG_F(INFO, "Adding linked audio to intermediate buffers");

			if (mixAudioToOutBuffer(fragment, left_buf, track->getGain(), track->getLevel()) != 0 ||
				mixAudioToOutBuffer(fragment, right_buf, track->getGain(), track->getLevel()) != 0)
			{
                LOG_F(ERROR, "Error occured while mixing");
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
    CodecFileInfo file_info(mix_path, arr, ses_len_smp, 1, g_session->getSampleRate());
    
    codec_info.save_file_proc(file_info, 2);

    delete[] left_buf;
    delete[] right_buf;

    return 0;
}