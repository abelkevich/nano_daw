#pragma once
#include "common.h"
#include <set>

struct Session
{
private:
    const uint32_t m_sample_rate;
    std::string m_name;
    std::string m_path;

    std::set<id_t> m_tracks;

public:
    Session(std::string name, std::string path, uint32_t sample_rate);

    uint32_t getSampleRate() const;

    std::string getName() const;
    status_t setName(std::string name);

    std::string getPath() const;
    status_t setPath(std::string path);

    std::set<id_t> getTracks() const;
    status_t linkTrack(id_t track_id);

    uint32_t msToSamples(uint32_t ms);
    uint32_t samplesToMs(uint32_t samples);
};
