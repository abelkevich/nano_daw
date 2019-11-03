#pragma once
#include "common.h"
#include "codec_file_info.h"
namespace CodecManager
{
    typedef status_t(__cdecl* LoadFileProc_t)(CodecFileInfo& codec_file_info, std::string path);
    typedef status_t(__cdecl* SaveFileProc_t)(CodecFileInfo codec_file_info, uint8_t bytes_per_sample);

    struct CodecInfo
    {
        std::string lib_name;
        LoadFileProc_t load_file_proc;
        SaveFileProc_t save_file_proc;

        CodecInfo(std::string _lib_name, LoadFileProc_t _load_file_proc, SaveFileProc_t _save_file_proc);
        CodecInfo();
    };

    status_t initCodecs();
    std::vector<CodecInfo> getInitedCodecs();
    status_t getCodec(std::string name, CodecInfo& codec_info);
}