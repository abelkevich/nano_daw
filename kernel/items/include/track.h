#pragma once
#include "common.h"
#include <set>

struct Track
{
private:
    std::set<id_t> m_fragments;
    std::set<id_t> m_effects;

    std::string m_name;
    bool m_mute;
    bool m_solo;
    int8_t m_pan;
    uint8_t m_gain;
    uint8_t m_level;

public:
    Track(std::string name);

    std::set<id_t> getFragments() const;
    std::set<id_t> getEffects() const;

    status_t unlinkFragment(id_t fragment_id);
    status_t linkFragment(id_t fragment_id);

    std::string getName() const;
    status_t setName(std::string name);

    bool getMute() const;
    status_t setMute(bool mute);

    bool getSolo() const;
    status_t setSolo(bool solo);

    int8_t getPan() const;
    status_t setPan(int8_t pan);

    uint8_t getGain() const;
    status_t setGain(uint8_t gain);

    uint8_t getLevel() const;
    status_t setLevel(uint8_t level);
};