#pragma once
#include "common.h"
#include <set>

typedef uint32_t id_t;

struct Effect
{
    const std::string name; // lib name
    std::string params; // serialized params

    Effect(std::string _name, std::string _params);
};

struct Audio
{
	const std::string path; // fs path 

	float *buffer; // decoded data
	const uint32_t buffer_size;

	Audio(std::string _path, float* _buffer, uint32_t _buffer_size);
};

struct Fragment
{
	const id_t linked_audio;

    uint32_t time_offset; // timeline start point
    uint32_t crop_from; // if zero -> disabled
    uint32_t crop_to;   // same

	Fragment(id_t _linked_audio);
};

struct Track
{
	std::set<id_t> fragments;
	std::set<id_t> effects;

    std::string name;
    bool mute;
    bool solo;
    int32_t pan;
    uint8_t gain;
    float level;

    Track(std::string _name);
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

    std::set<id_t> tracks;

    Session(std::string _name, std::string _path, uint32_t _sample_rate);
};