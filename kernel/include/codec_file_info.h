#pragma once

#include "common.h"

struct CodecFileInfo
{
    std::string path;
    float** buffers;
    uint32_t samples_per_channel;
    uint32_t sample_rate;
    uint8_t channel_num;

    CodecFileInfo(std::string _path, float** _buffers, uint32_t _samples_per_channel, 
                  uint8_t _channel_num, uint32_t _sample_rate);

    CodecFileInfo();
};