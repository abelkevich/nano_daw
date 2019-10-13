#pragma once
#include "common.h"

namespace Codecs::PureWave
{
#pragma pack(push, 1)
    struct WavHeader {
        // RIFF Header
        char riff_header[4]; // Contains "RIFF"
        uint32_t wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
        char wave_header[4]; // Contains "WAVE"

        // Format Header
        char fmt_header[4]; // Contains "fmt " (includes trailing space)
        uint32_t fmt_chunk_size; // Should be 16 for PCM
        uint16_t audio_format; // Should be 1 for PCM. 3 for IEEE Float
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
        uint16_t sample_alignment; // num_channels * Bytes Per Sample
        uint16_t bit_depth; // Number of bits per sample

        // Data
        char data_header[4]; // Contains "data"
        uint32_t data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    };
#pragma pack(pop)

    typedef struct wav_header wav_header;
    typedef int16_t level_t;

    EXPORTED uint8_t readHeader(FILE* f, WavHeader& header);
    EXPORTED uint8_t readData(FILE* f, const WavHeader& header, level_t* buffer);
    EXPORTED uint8_t writeHeader(FILE* f, const WavHeader& header);
    EXPORTED uint8_t writeData(FILE* f, const WavHeader& header, const level_t* buffer);
    EXPORTED uint8_t allocBuffer(const WavHeader& header, level_t** buffer);
    EXPORTED void freeBuffer(level_t* buffer);
    EXPORTED WavHeader makeDefaultHeader(uint32_t sample_rate, uint32_t buf_size);
};