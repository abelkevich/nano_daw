#pragma once
#include "common.h"
#include <set>


struct Fragment
{
private:
    const id_t m_linked_audio;
    const id_t m_linked_track;
    const id_t m_id;

    uint32_t m_time_offset; // timeline start point
    uint32_t m_crop_from; // if zero -> disabled
    uint32_t m_crop_to;   // same

public:
    Fragment(const id_t id, const id_t linked_track, const id_t linked_audio);

    ms_t getTimelineOffset() const;
    bool setTimelineOffset(ms_t offset);

    ms_t getCropFrom() const;
    ms_t getCropTo() const;

    id_t getTrack() const;
    id_t getAudio() const;

    bool crop(ms_t crop_from, ms_t crop_to);
    ms_t getAudioLength() const;

    id_t getId() const;
};