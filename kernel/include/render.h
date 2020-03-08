#pragma once
#include "common.h"

bool render(const id_t session_id, const std::string& mix_path);
std::set<id_t> getTracksWithSolo(const id_t session_id);
bool mixAudioToOutBuffer(const id_t fragment_id, float* out_buf, const smpn_t ses_len_smp, const uint8_t gain, const uint8_t level);
ms_t calcSessionLength(const id_t session_id);