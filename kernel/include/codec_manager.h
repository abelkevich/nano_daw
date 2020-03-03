#pragma once
#include "common.h"
#include "codec_file_info.h"
#ifdef __linux__
#include <dlfcn.h>
#else
#include <windows.h>
#endif

namespace CodecManager
{
    typedef status_t(__cdecl* LoadFileProc_t)(CodecFileInfo& codec_file_info, std::string path);
    typedef status_t(__cdecl* SaveFileProc_t)(CodecFileInfo codec_file_info, uint8_t bytes_per_sample);
    typedef const char*(__cdecl* GetName_t)();
    typedef const char* (__cdecl* GetExtensions_t)();
#ifdef __linux__
    struct Codec
    {
        LoadFileProc_t loadFile;
        SaveFileProc_t saveFile;
        GetName_t getName;
        GetExtensions_t getExtensions;


        void* h_instance;

        Codec(LoadFileProc_t _loadFile, SaveFileProc_t _saveFile, GetName_t _getName, GetExtensions_t _getExtensions, void* _h_instance);
    };
#else
    struct Codec
    {
        LoadFileProc_t loadFile;
        SaveFileProc_t saveFile;
        GetName_t getName;
        GetExtensions_t getExtensions;


        HINSTANCE h_instance;

        Codec(LoadFileProc_t _loadFile, SaveFileProc_t _saveFile, GetName_t _getName, GetExtensions_t _getExtensions, HINSTANCE _h_instance);
    };
#endif

    status_t initCodecs();
    std::vector<const Codec*> getInitedCodecs();
    const Codec* getCodecByName(std::string name);
    const Codec* findCodecByFileExt(std::string ext);
}