#include "codec_file_info.h"

CodecFileInfo::CodecFileInfo(std::string _path, float** _buffers, uint32_t _samples_per_channel, uint8_t _channel_num, uint32_t _sample_rate)
                           : path(_path)
                           , buffers(_buffers)
                           , samples_per_channel(_samples_per_channel)
                           , channel_num(_channel_num)
                           , sample_rate(_sample_rate)
{
}

CodecFileInfo::CodecFileInfo()
            : buffers(nullptr)
            , samples_per_channel(0)
            , channel_num(0)
            , sample_rate(0)
{

}