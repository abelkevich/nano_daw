#include "session.h"
#include "codec_manager.h"

Session::Session()
    : state(ESessionState::eNonInited)
    , sample_rate(0)
{

}

Session::Session(std::string _name, std::string _path, uint32_t _sample_rate, std::vector<Track> _tracks)
    : state(ESessionState::eIdle)
    , name(_name)
    , path(_path)
    , tracks(_tracks)
    , sample_rate(_sample_rate)
{

}

Track* Session::getTrack(const std::string& name)
{
    for (auto t : tracks)
    {
        if (t.name == name)
        {
            return &t;
        }
    }

    return nullptr;
}

Track::Track(std::string _name, bool _mute, bool _solo, int32_t _pan, uint8_t _gain, float _level, std::vector<Audio> _audio, std::vector<Effect> _effects)
    : name(_name)
    , mute(_mute)
    , solo(_solo)
    , pan(_pan)
    , gain(_gain)
    , level(_level)
    , audio(_audio)
    , effects(_effects)
{
}

Audio::Audio(std::string _path, uint32_t _time_offset, uint32_t _crop_from, uint32_t _crop_to, float* _buffer, uint32_t _buffer_size)
    : path(_path)
    , time_offset(_time_offset)
    , crop_from(_crop_from)
    , crop_to(_crop_to)
    , buffer(_buffer)
    , buffer_size(_buffer_size)
{

}

Effect::Effect(std::string _name, std::string _params)
    : name(_name)
    , params(_params)
{

}

Session genDummySession()
{
    CodecInfo codec;

    if (getCodec("pure_wave.dll", codec) != 0)
    {
        return Session("none", "none", 0, {});
    }

    auto loadAudio = [=](std::string path) -> Audio
    {
        CodecFileInfo file_info;

        if (codec.load_file_proc(file_info, path))
        {
        }
        
        return Audio(path, 0, 0, 20000, file_info.buffers[0], file_info.samples_per_channel);
    };

    Audio audio_1 = loadAudio("res\\samples_16bit_48khz\\bass_di.wav");
    Audio audio_2 = loadAudio("res\\samples_16bit_48khz\\drums_overhead_l.wav");
    Audio audio_3 = loadAudio("res\\samples_16bit_48khz\\drums_overhead_r.wav");
    Audio audio_4 = loadAudio("res\\samples_16bit_48khz\\drums_kick.wav");
    Audio audio_5 = loadAudio("res\\samples_16bit_48khz\\sax.wav");

    Effect effect_1("echo", "100ms");

    Track track_1("none", false, false, 0, 1, 0, {audio_1, audio_2}, {});
    Track track_2("none", false, false, 0, 1, 0, {audio_3, audio_4, audio_5}, {effect_1});

    Session session("sample", "sample.proj", 48000, {track_1, track_2});

    return session;
}