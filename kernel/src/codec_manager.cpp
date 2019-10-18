#include "codec_manager.h"
#include <vector>
#include <windows.h> 

static std::vector<CodecInfo> g_codecs;

CodecInfo::CodecInfo(std::string _lib_name, LoadFileProc_t _load_file_proc, SaveFileProc_t _save_file_proc)
    : lib_name(_lib_name)
    , load_file_proc(_load_file_proc)
    , save_file_proc(_save_file_proc)
{
}

CodecInfo::CodecInfo()
    : load_file_proc(nullptr)
    , save_file_proc(nullptr)
{
}

status_t initCodecs()
{
    HINSTANCE hinstLib = LoadLibrary(TEXT("pure_wave.dll"));
    
    if (!hinstLib)
    {
        return 1;
    }

    LoadFileProc_t load_file_proc;
    SaveFileProc_t save_file_proc;

    load_file_proc = (LoadFileProc_t) GetProcAddress(hinstLib, "loadFile");
    save_file_proc = (SaveFileProc_t) GetProcAddress(hinstLib, "saveFile");

    if (!load_file_proc || !save_file_proc)
    {
        return 2;
    }

    g_codecs.push_back(CodecInfo("pure_wave.dll", load_file_proc, save_file_proc));
    
    return 0;
}

std::vector<CodecInfo> getInitedCodecs()
{
    return g_codecs;
}

status_t getCodec(std::string name, CodecInfo &codec_info)
{
    for (auto codec : g_codecs)
    {
        if (codec.lib_name == name)
        {
            codec_info = codec;
            return 0;
        }
    }

    return 1;
}