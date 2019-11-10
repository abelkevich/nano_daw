#pragma once
#include "common.h"
#include <set>

struct Audio
{
private:
    const std::string m_path; // fs path 
    float* m_buffer; // decoded data
    const uint32_t m_buffer_size;

public:
    Audio(std::string path, float* buffer, uint32_t buffer_size);

    std::string getPath() const;
    uint32_t getBufferLength() const;
    float* getBuffer() const;
};