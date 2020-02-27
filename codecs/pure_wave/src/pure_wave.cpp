#include <cstdlib>
#include <cmath>
#include <cstdio>
#include "pure_wave.h"
#include <cstring>//gcc didn`t compile memcpy without this lib



static status_t readHeader(FILE* f, WavHeader& header)
{
    if (!f)
    {
        return 1;
    }

    fseek(f, 0, SEEK_SET);
    size_t elements_read = fread(&header, sizeof(WavHeader), 1, f);

    if (elements_read != 1)
    {
        return 2;
    }

    return 0;
}

static status_t readData(FILE* f, const WavHeader& header, uint8_t* buffer)
{
    if (!f || !buffer)
    {
        return 1;
    }

    fseek(f, sizeof(WavHeader), SEEK_SET);
    size_t elements_read = fread(buffer, header.data_bytes, 1, f);

    if (elements_read != 1)
    {
        return 2;
    }

    return 0;
}

static status_t writeHeader(FILE* f, const WavHeader& header)
{
    if (!f)
    {
        return 1;
    }

    fseek(f, 0, SEEK_SET);

    size_t elements_writed = fwrite(&header, sizeof(WavHeader), 1, f);

    if (elements_writed != 1)
    {
        return 2;
    }

    return 0;
}

static status_t writeData(FILE* f, const WavHeader& header, const uint8_t* buffer)
{
    if (!f || !buffer)
    {
        return 1;
    }

    fseek(f, sizeof(WavHeader), SEEK_SET);
    size_t elements_writed = fwrite(buffer, header.data_bytes, 1, f);

    if (elements_writed != 1)
    {
        return 2;
    }

    return 0;
}

static status_t allocBuffer(const WavHeader& header, uint8_t** buffer)
{
    if (!buffer)
    {
        return 1;
    }

    *buffer = new uint8_t[header.data_bytes];

    if (!*buffer)
    {
        return 2;
    }

    return 0;
}

static void freeBuffer(uint8_t* buffer)
{
    free(buffer);
}

int32_t getBytes(uint8_t* buffer, uint8_t n)
{
    if (n > 4) return 0;
    
    int32_t signed_out_val = 0;

    memcpy(&signed_out_val, buffer, n);

    if (signed_out_val > pow(2, n * 8) / 2)
    {
        signed_out_val |= 0xFFFFFFFF << n * 8;
    }

    return signed_out_val;
}

void setBytes(uint8_t* buffer, uint8_t n, int32_t val)
{
    if (n > 4) return;

    memcpy(buffer, &val, n);
}

status_t loadFile(CodecFileInfo& codec_file_info, std::string path)
{
    // Read .wav contents
    FILE* f = fopen(path.c_str(), "rb");

    if (!f)
    {
        return 1;
    }

    WavHeader wav_header;
    if (readHeader(f, wav_header) != 0)
    {
        fclose(f);
        return 2;
    }

    uint8_t* buffer = nullptr;
    if (allocBuffer(wav_header, &buffer) != 0)
    {
        fclose(f);
        return 3;
    }

    if (readData(f, wav_header, buffer) != 0)
    {
        fclose(f);
        return 4;
    }

    fclose(f);

    // Calc info
    uint8_t bytes_per_sample = wav_header.bit_depth / 8;
    uint32_t samples_per_channel = wav_header.data_bytes / bytes_per_sample / wav_header.num_channels;

    float** splitted_buffers = new float* [wav_header.num_channels];
    for (uint8_t ch_ind = 0; ch_ind < wav_header.num_channels; ch_ind++)
    {
        splitted_buffers[ch_ind] = new float[samples_per_channel];

        for (uint32_t smp_ind = 0; smp_ind < samples_per_channel; smp_ind++)
        {
            uint32_t max_sample_val = pow(2, bytes_per_sample * 8);
           
            uint32_t offset = (ch_ind + 1) * smp_ind * bytes_per_sample;
            int32_t val = getBytes(buffer + offset, bytes_per_sample);

            float normilized_val = (float) val / max_sample_val;

            splitted_buffers[ch_ind][smp_ind] = normilized_val;
        }
    }

    delete[] buffer;

    codec_file_info = CodecFileInfo(path, splitted_buffers, samples_per_channel
                                        , wav_header.num_channels, wav_header.sample_rate);

    return 0;
}

status_t saveFile(CodecFileInfo codec_file_info, uint8_t bytes_per_sample)
{
    uint32_t buffer_size = codec_file_info.channel_num * codec_file_info.samples_per_channel * bytes_per_sample;
    uint8_t * buffer = new uint8_t[buffer_size];

    for (uint8_t ch_ind = 0; ch_ind < codec_file_info.channel_num; ch_ind++)
    {
        for (uint32_t smp_ind = 0; smp_ind < codec_file_info.samples_per_channel; smp_ind++)
        {
            int32_t fixed_val = 0;
            uint32_t max_sample_val = pow(2, bytes_per_sample * 8);

            fixed_val = codec_file_info.buffers[ch_ind][smp_ind] * max_sample_val;
            uint32_t offset = (ch_ind + 1) * smp_ind * bytes_per_sample;

            setBytes(buffer + offset, bytes_per_sample, fixed_val);
        }
    }

    WavHeader wav_header(codec_file_info.samples_per_channel, codec_file_info.channel_num, codec_file_info.sample_rate, bytes_per_sample);

    FILE* file_mix = fopen(codec_file_info.path.c_str(), "wb");

    writeHeader(file_mix, wav_header);
    writeData(file_mix, wav_header, buffer);

    fclose(file_mix);

    // free mix buf
    delete[] buffer;

    return 0;
}

const char* getName()
{
    return "pure_wave";
}

const char* getExtensions()
{
    return "wav";
}