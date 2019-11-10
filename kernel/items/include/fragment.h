#pragma once
#include "common.h"
#include <set>


struct Fragment
{
private:
    const id_t m_linked_audio;

    uint32_t m_time_offset; // timeline start point
    uint32_t m_crop_from; // if zero -> disabled
    uint32_t m_crop_to;   // same

public:
    Fragment(id_t linked_audio);

    uint32_t getTimeOffset() const;
    status_t setTimeOffset(uint32_t offset);

    uint32_t getCropFrom() const;
    uint32_t getCropTo() const;

    id_t getAudio() const;

    status_t crop(uint32_t crop_from, uint32_t crop_to);
};