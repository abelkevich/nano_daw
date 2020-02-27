#pragma once
#include "codec_api.h"

#pragma pack(push, 1)
    struct WavHeader {
        // RIFF Header
        const char riff_header[4] = {'R', 'I', 'F', 'F'};
        uint32_t wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
        const char wave_header[4] = {'W', 'A', 'V', 'E'};

        // Format Header
        const char fmt_header[4] = { 'f', 'm', 't', ' ' };
        const uint32_t fmt_chunk_size = 16; // Should be 16 for PCM
        const uint16_t audio_format = 1; // Should be 1 for PCM. 3 for IEEE Float
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate; // Number of bytes per second. sample_rate * num_channels * bytes per sample
        uint16_t sample_alignment; // num_channels * bytes per sample
        uint16_t bit_depth; // Number of bits per sample

        // Data
        const char data_header[4] = {'d', 'a', 't', 'a'};
        uint32_t data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size

        WavHeader(uint32_t _samples_per_channel, uint8_t _num_channels, uint32_t _sample_rate, uint8_t _bytes_per_sample)
                : bit_depth(_bytes_per_sample * 8)
                , num_channels(_num_channels)
                , sample_rate(_sample_rate)
                , byte_rate(_sample_rate * _num_channels * _bytes_per_sample)
                , sample_alignment(_bytes_per_sample * _num_channels)
                , data_bytes(_samples_per_channel * _num_channels * _bytes_per_sample)
                , wav_size(4 + 24 + 8 + _samples_per_channel * _num_channels * _bytes_per_sample)
        {
            
        }

        WavHeader()
            : bit_depth(0)
            , num_channels(0)
            , sample_rate(0)
            , byte_rate(0)
            , sample_alignment(0)
            , data_bytes(0)
            , wav_size(0)
        {
        }
    };
#pragma pack(pop)

    static uint8_t readHeader(FILE* f, WavHeader& header);
    static uint8_t readData(FILE* f, const WavHeader& header, uint8_t* buffer);
    static uint8_t writeHeader(FILE* f, const WavHeader& header);
    static uint8_t writeData(FILE* f, const WavHeader& header, const uint8_t* buffer);
    static uint8_t allocBuffer(const WavHeader& header, uint8_t** buffer);
    static void freeBuffer(uint8_t* buffer);
    EXPORTED int32_t getBytes(uint8_t* buffer, uint8_t n);
    EXPORTED void setBytes(uint8_t* buffer, uint8_t n, int32_t val);