#include "session.h"
#include "codec_manager.h"

Session::Session(std::string _name, std::string _path, uint32_t _sample_rate)
    : state(ESessionState::eIdle)
    , name(_name)
    , path(_path)
    , sample_rate(_sample_rate)
{
}

Track::Track(std::string _name)
    : name(_name)
    , mute(false)
    , solo(false)
    , pan(0)
    , gain(0)
    , level(0)
{
}

Audio::Audio(std::string _path, float* _buffer, uint32_t _buffer_size)
    : path(_path)
    , buffer(_buffer)
    , buffer_size(_buffer_size)
{
}

Fragment::Fragment(id_t _linked_audio)
	: linked_audio(_linked_audio)
	, time_offset(0)
	, crop_from(0)
	, crop_to(0)
{
}

Effect::Effect(std::string _name, std::string _params)
    : name(_name)
    , params(_params)
{
}