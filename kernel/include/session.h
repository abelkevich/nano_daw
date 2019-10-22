#pragma once
#include "common.h"
#include <map>

typedef uint32_t id_t;


struct Effect
{
    const id_t id;
    const std::string name; // lib name
    std::string params; // serialized params

    Effect(std::string _name, std::string _params);
};

struct Audio
{
    const std::string path; // fs path 
    const id_t id;

    uint32_t time_offset; // timeline start point
    uint32_t crop_from; // if zero -> disabled
    uint32_t crop_to;   // same

    float *buffer; // decoded data
    const uint32_t buffer_size;

    Audio(std::string _path, float* _buffer, uint32_t _buffer_size);
};

struct Track
{
    std::map<id_t, Audio> audio;
    std::map<id_t, Effect> effects;

    const id_t id;

    std::string name;
    bool mute;
    bool solo;
    int32_t pan;
    uint8_t gain;
    float level;

    Track(std::string _name);

    status_t addAudio(const Audio &audio);
    Audio* getAudio(id_t id);
    status_t removeAudio(id_t);
    
    status_t addEffect(const Effect& audio);
    Effect* getEffect(id_t id);
    status_t removeEffect(id_t id);
};

enum class ESessionState
{
    ePlay, eRecord, eIdle, eNonInited
};

struct Session
{
    ESessionState state;
  
    const uint32_t sample_rate;

    std::string name;
    const std::string path;

    std::map<id_t, Track> tracks;

    Session(std::string _name, std::string _path, uint32_t _sample_rate);

    status_t addTrack(const Track& track);
    Track* getTrack(id_t id);
    status_t removeTrack(id_t id);
};

Session* genDummySession();