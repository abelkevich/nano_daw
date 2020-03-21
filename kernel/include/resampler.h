#include <cinttypes>
#include "common.h"

namespace Resampler
{
    float resampleFactor(const freq_t freq_original, const freq_t freq_target);
    bool decimation(const uint8_t k, const float* signal_original, const smpn_t samples_original, float** _signal_decimated, smpn_t* _samples_decimated);
    bool interpolation(const uint8_t k, const float* signal_original, const smpn_t samples_original, float** _signal_interpolated, smpn_t* _samples_interpolated);
    bool resample(const freq_t freq_original, const freq_t freq_target, const float* signal_original, const smpn_t samples_original, float** _signal_resampled, smpn_t* _samples_resampled);
}