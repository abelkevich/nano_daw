#pragma once
#include "common.h"
#include "codec_file_info.h"

#ifdef __cplusplus 
extern "C" {
#endif

EXPORTED status_t __cdecl loadFile(CodecFileInfo &codec_file_info, std::string path);
EXPORTED status_t __cdecl saveFile(CodecFileInfo codec_file_info, uint8_t bytes_per_sample);
EXPORTED const char* __cdecl getName();
EXPORTED const char* __cdecl getExtensions();

#ifdef __cplusplus
}
#endif