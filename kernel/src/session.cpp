#include "session.h"
#include "codec_manager.h"

id_t genUniqueId()
{
    static id_t n = 0;
    return n++;
}

Session::Session(std::string _name, std::string _path, uint32_t _sample_rate)
    : state(ESessionState::eIdle)
    , name(_name)
    , path(_path)
    , sample_rate(_sample_rate)
{

}

Track* Session::getTrack(id_t id)
{
    if (tracks.find(id) == tracks.end())
    {
        return nullptr;
    }

    return &(tracks.at(id));
}

status_t Session::addTrack(const Track& track)
{
    if (tracks.find(track.id) != tracks.end())
    {
        return 1;
    }

    auto rec = std::make_pair(track.id, track);
    tracks.insert(rec);

    return 0;
}

status_t Session::removeTrack(id_t id)
{
    if (tracks.find(id) == tracks.end())
    {
        return 1;
    }

    tracks.erase(id);

    return 0;
}

Track::Track(std::string _name)
    : id(genUniqueId())
    , name(_name)
    , mute(false)
    , solo(false)
    , pan(0)
    , gain(0)
    , level(0)
{
}

Audio* Track::getAudio(id_t id)
{
    if (audio.find(id) == audio.end())
    {
        return nullptr;
    }

    return &(audio.at(id));
}

status_t Track::addAudio(const Audio& a)
{
    if (audio.find(a.id) != audio.end())
    {
        return 1;
    }

    auto rec = std::make_pair(a.id, a);
    audio.insert(rec);

    return 0;
}

status_t Track::removeAudio(id_t id)
{
    if (audio.find(id) == audio.end())
    {
        return 1;
    }

    audio.erase(id);

    return 0;
}

Effect* Track::getEffect(id_t id)
{
    if (effects.find(id) == effects.end())
    {
        return nullptr;
    }

    return &(effects.at(id));
}

status_t Track::addEffect(const Effect& effect)
{
    if (effects.find(effect.id) != effects.end())
    {
        return 1;
    }

    auto rec = std::make_pair(effect.id, effect);
    effects.insert(rec);

    return 0;
}

status_t Track::removeEffect(id_t id)
{
    if (effects.find(id) == effects.end())
    {
        return 1;
    }

    effects.erase(id);

    return 0;
}


Audio::Audio(std::string _path, float* _buffer, uint32_t _buffer_size)
    : id(genUniqueId())
    , path(_path)
    , time_offset(0)
    , crop_from(0)
    , crop_to(0)
    , buffer(_buffer)
    , buffer_size(_buffer_size)
{

}

Effect::Effect(std::string _name, std::string _params)
    : id(genUniqueId())
    , name(_name)
    , params(_params)
{

}

Session* genDummySession()
{
    CodecInfo codec;

    if (getCodec("pure_wave.dll", codec) != 0)
    {
        return new Session("none", "none", 0);
    }

    auto loadAudio = [=](std::string path) -> Audio
    {
        CodecFileInfo file_info;

        if (codec.load_file_proc(file_info, path))
        {
        }
        
        return Audio(path, file_info.buffers[0], file_info.samples_per_channel);
    };

    Audio audio_1 = loadAudio("res\\samples_16bit_48khz\\bass_di.wav");
    Audio audio_2 = loadAudio("res\\samples_16bit_48khz\\drums_overhead_l.wav");
    Audio audio_3 = loadAudio("res\\samples_16bit_48khz\\drums_overhead_r.wav");
    Audio audio_4 = loadAudio("res\\samples_16bit_48khz\\drums_kick.wav");
    Audio audio_5 = loadAudio("res\\samples_16bit_48khz\\sax.wav");

    Effect effect_1("echo", "100ms");

    Track track_1("none");
    track_1.addAudio(audio_1);
    track_1.addEffect(effect_1);

    Track track_2("none");
    track_2.addAudio(audio_2);
    track_2.addAudio(audio_3);

    return new Session("sample", "sample.proj", 48000);
}