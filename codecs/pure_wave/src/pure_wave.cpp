#include <cstdlib>
#include "pure_wave.h"

namespace Codecs::PureWave
{
    __declspec(dllexport) status_t readHeader(FILE* f, WavHeader& header)
    {
        if (!f)
        {
            return 0;
        }

        fseek(f, 0, SEEK_SET);
        size_t elements_read = fread(&header, sizeof(WavHeader), 1, f);

        if (elements_read != 1)
        {
            return 0;
        }

        return 1;
    }

    __declspec(dllexport) status_t readData(FILE* f, const WavHeader& header, level_t* buffer)
    {
        if (!f || !buffer)
        {
            return 0;
        }

        fseek(f, sizeof(WavHeader), SEEK_SET);
        size_t elements_read = fread(buffer, header.data_bytes, 1, f);

        if (elements_read != 1)
        {
            return 0;
        }

        return 1;
    }

    __declspec(dllexport) status_t writeHeader(FILE* f, const WavHeader& header)
    {
        if (!f)
        {
            return 0;
        }

        fseek(f, 0, SEEK_SET);

        size_t elements_writed = fwrite(&header, sizeof(WavHeader), 1, f);

        if (elements_writed != 1)
        {
            return 0;
        }

        return 1;
    }

    __declspec(dllexport) status_t writeData(FILE* f, const WavHeader& header, const level_t* buffer)
    {
        if (!f || !buffer)
        {
            return 0;
        }

        fseek(f, sizeof(WavHeader), SEEK_SET);
        size_t elements_writed = fwrite(buffer, header.data_bytes, 1, f);

        if (elements_writed != 1)
        {
            return 0;
        }

        return 1;
    }

    __declspec(dllexport) status_t allocBuffer(const WavHeader& header, level_t** buffer)
    {
        if (!buffer)
        {
            return 0;
        }

        *buffer = (level_t*)malloc(header.data_bytes);

        if (!*buffer)
        {
            return 0;
        }

        return 1;
    }

    __declspec(dllexport) void freeBuffer(level_t* buffer)
    {
        free(buffer);
    }

    __declspec(dllexport) WavHeader makeDefaultHeader(uint32_t sample_rate, uint32_t buf_size)
    {
        WavHeader h =
        {
            {'R', 'I', 'F', 'F'}, buf_size * 2, {'W', 'A', 'V', 'E'}, {'f', 'm', 't', ' '},
            16, 1, 2, sample_rate, sample_rate * 2 * 2, 1, 16, {'d', 'a', 't', 'a'}, buf_size * 2 + 182
        };

        return h;
    }
};