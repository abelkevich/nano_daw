#include "audio.h"

Audio::Audio(std::string path, float* buffer, uint32_t buffer_size)
    : m_path(path)
    , m_buffer(buffer)
    , m_buffer_size(buffer_size)
{
}


std::string Audio::getPath() const { return m_path; }
uint32_t Audio::getBufferLength() const { return m_buffer_size; }
float* Audio::getBuffer() const { return m_buffer; }