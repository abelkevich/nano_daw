#pragma once
#include "common.h"
#include <set>

struct Session
{
private:
    const id_t m_id;
    const freq_t m_sample_rate;
    std::string m_name;
    std::string m_path;

    std::set<id_t> m_tracks;

public:
    Session(const id_t id, const std::string &name, const std::string &path, const freq_t sample_rate);

    freq_t getSampleRate() const;

    std::string getName() const;
    bool setName(const std::string &name);

    std::string getPath() const;
    bool setPath(const std::string &path);

    std::set<id_t> getTracks() const;
    bool linkTrack(id_t track_id);
    bool unlinkTrack(id_t track_id);

    smpn_t msToSamples(ms_t ms) const;
    ms_t samplesToMs(smpn_t samples) const;

    id_t getId() const;
};
