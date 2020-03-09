#pragma once
#include "common.h"
#include "codec_file_info.h"

namespace CodecManager
{
    typedef status_t(__cdecl* LoadFileProc_t)(CodecFileInfo& codec_file_info, std::string path);
    typedef status_t(__cdecl* SaveFileProc_t)(CodecFileInfo codec_file_info, uint8_t bytes_per_sample);
    typedef const char*(__cdecl* GetName_t)();
    typedef const char* (__cdecl* GetExtensions_t)();

    struct Codec
    {
        LoadFileProc_t loadFile;
        SaveFileProc_t saveFile;
        GetName_t getName;
        GetExtensions_t getExtensions;

        void* h_instance;

        Codec(LoadFileProc_t _loadFile, SaveFileProc_t _saveFile, GetName_t _getName, GetExtensions_t _getExtensions, void* _h_instance);
    };

    bool initCodecs();
    std::vector<const Codec*> getInitedCodecs();
    const Codec* getCodecByName(std::string name);
    const Codec* findCodecByFileExt(std::string ext);
}