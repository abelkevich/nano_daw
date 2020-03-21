#include "resampler.h"

#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace Resampler
{
    static float* duplicate(const float* signal_src, const smpn_t samples_n)
    {
        float* signal_duplicate = new float[samples_n];
        memcpy(signal_duplicate, signal_src, samples_n * sizeof(float));
        return signal_duplicate;
    }


    bool decimation(const uint8_t k, const float* signal_original, const smpn_t samples_original, float** _signal_decimated, smpn_t* _samples_decimated)
    {
        if (k == 0 || k == 1)
        {
            return false;
        }

        if (!signal_original)
        {
            return false;
        }

        const smpn_t samples_decimated = samples_original / k;
        float* signal_decimated = new float[samples_decimated];

        if (!signal_decimated)
        {
            return false;
        }

        // interpolation
        for (uint32_t orig_smp_ind = 0, dcmtd_smp_ind = 0; orig_smp_ind < samples_original; orig_smp_ind += k, dcmtd_smp_ind++)
        {
            signal_decimated[dcmtd_smp_ind] = signal_original[orig_smp_ind];
        }


        *_signal_decimated = signal_decimated;
        *_samples_decimated = samples_decimated;

        return true;
    }

    bool interpolation(const uint8_t k, const float* signal_original, const smpn_t samples_original, float** _signal_interpolated, smpn_t* _samples_interpolated)
    {
        if (k == 0 || k == 1)
        {
            return false;
        }

        if (!signal_original)
        {
            return false;
        }

        const smpn_t samples_interpolated = samples_original * k;
        float* signal_interpolated = new float[samples_interpolated];

        if (!signal_interpolated)
        {
            return false;
        }

        // interpolation
        for (smpn_t orig_smp_ind = 0, intrp_smp_ind = 0; orig_smp_ind < samples_original; orig_smp_ind++)
        {
            const float f0 = signal_original[orig_smp_ind];
            const float f1 = (orig_smp_ind != samples_original - 1) ?
                            signal_original[orig_smp_ind + 1] :
                            signal_original[orig_smp_ind];

            const float x0 = (float) orig_smp_ind;
            const float x1 = (float) orig_smp_ind + k;

            const float c1 = (f1 - f0) / (x1 - x0);
            const float c2 = c1 * x0 * (-1) + f0;

            for (uint8_t k_ind = 0; k_ind < k; k_ind++)
            {
                const float x = x0 + k_ind;
                const float y = x * c1 + c2;

                signal_interpolated[intrp_smp_ind] = y;
                intrp_smp_ind++;
            }
        }


        *_signal_interpolated = signal_interpolated;
        *_samples_interpolated = samples_interpolated;

        return true;
    }

    float resampleFactor(const freq_t freq_original, const freq_t freq_target)
    {
        return (float) freq_target / freq_original;
    }

    bool resample(const freq_t freq_original, const freq_t freq_target, const float* signal_original, const smpn_t samples_original, float** _signal_resampled, smpn_t* _samples_resampled)
    {
        freq_t M = freq_target;
        freq_t N = freq_original;

        auto findPossibleCommon = [=](freq_t M, freq_t N) -> uint32_t
        {
            for (uint32_t possible_common = std::max(M, N); possible_common > 1; possible_common--)
            {
                if (M % possible_common == 0 &&
                    N % possible_common == 0)
                {
                    return possible_common;
                }
            }

            return 0;
        };

        uint32_t common = findPossibleCommon(M, N);
        while (common != 0)
        {
            M /= common;
            N /= common;

            common = findPossibleCommon(M, N);
        }

        if (M == 0 || N == 0)
        {
            return false;
        }

        float* signal_interpolated = nullptr;
        smpn_t samples_interpolated = 0;

        if (M == 1)
        {
            signal_interpolated = duplicate(signal_original, samples_original);
            samples_interpolated = samples_original;
        }
        else if (!interpolation(M, signal_original, samples_original, &signal_interpolated, &samples_interpolated))
        {
            return false;
        }

        float* signal_decimated = nullptr;
        smpn_t samples_decimated = 0;
        
        if (N == 1)
        {
            signal_decimated = duplicate(signal_interpolated, samples_interpolated);
            samples_decimated = samples_interpolated;
        }
        else if (!decimation(N, signal_interpolated, samples_interpolated, &signal_decimated, &samples_decimated))
        {
            delete[] signal_interpolated;
            return false;
        }

        delete[] signal_interpolated;

        *_signal_resampled = signal_decimated;
        *_samples_resampled = samples_decimated;

        return true;
    }

}