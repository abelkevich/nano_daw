#pragma once
#include "common.h"

struct Effect
{
    std::string name; // lib name
    std::string params; // serialized params

    Effect(std::string _name, std::string _params)
        : name(_name)
        , params(_params)
    {

    }
};

struct Audio
{
    std::string path; // fs path 

    uint32_t time_offset; // timeline start point
    uint32_t crop_from; // if zero -> disabled
    uint32_t crop_to;   // same

    float *buffer; // decoded data
    uint32_t buffer_size;

    Audio(std::string _path, uint32_t _time_offset, uint32_t _crop_from, uint32_t _crop_to, float* _buffer, uint32_t _buffer_size)
        : path(_path)
        , time_offset(_time_offset)
        , crop_from(_crop_from)
        , crop_to(_crop_to)
        , buffer(_buffer)
        , buffer_size(_buffer_size)
    {

    }
};

struct Track
{
    std::vector<Audio> audio;
    std::vector<Effect> effects;

    std::string name;
  
    bool mute;
    bool solo;
    int32_t pan;
    uint8_t gain;
    float level;

    Track(bool _mute, bool _solo, int32_t _pan, uint8_t _gain, float _level, std::vector<Audio> _audio, std::vector<Effect> _effects)
        : mute(_mute)
        , solo(_solo)
        , pan(_pan)
        , gain(_gain)
        , level(_level)
        , audio(_audio)
        , effects(_effects)
    {

    }
};

enum class ESessionState
{
    ePlay, eRecord, eIdle, eNonInited
};

struct Session
{
    ESessionState state;
  
    uint32_t sample_rate;

    std::string name;
    std::string path;

    std::vector<Track> tracks;

    Session()
        : state(ESessionState::eNonInited)
        , sample_rate(0)
    {

    }

    Session(std::string _name, std::string _path, uint32_t _sample_rate, std::vector<Track> _tracks)
        : state(ESessionState::eIdle)
        , name(_name)
        , path(_path)
        , tracks(_tracks)
        , sample_rate(_sample_rate)
    {

    }

};

Session genDummySession();