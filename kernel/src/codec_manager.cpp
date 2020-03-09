#include "codec_manager.h"
#include "kernel.h"
#include "utils.h"

#include <vector>
#include <iostream>
#include <list>
#include <string>

namespace CodecManager
{
    static std::vector<Codec> g_codecs;

#ifdef __linux__
    Codec::Codec(LoadFileProc_t _loadFile, SaveFileProc_t _saveFile, GetName_t _getName, GetExtensions_t _getExtensions, void* _h_instance)
               : loadFile(_loadFile)
               , saveFile(_saveFile)
               , getName(_getName)
               , getExtensions(_getExtensions)
               , h_instance(_h_instance)
    {
    }

    status_t addCodec(std::string path)
    {
        void* h_instance = dlopen(path.c_str(), RTLD_LAZY);

        if (!h_instance)
        {
            return 1;
        }

        LoadFileProc_t load_file_proc = (LoadFileProc_t)dlsym(h_instance, "loadFile");
        SaveFileProc_t save_file_proc = (SaveFileProc_t)dlsym(h_instance, "saveFile");
        GetName_t get_name_proc = (GetName_t)dlsym(h_instance, "getName");
        GetExtensions_t get_ext_proc = (GetExtensions_t)dlsym(h_instance, "getExtensions");

        if (!load_file_proc || !save_file_proc || !get_name_proc || !get_ext_proc)
        {
            return 2;
        }

        Codec codec(load_file_proc, save_file_proc, get_name_proc, get_ext_proc, h_instance);

        g_codecs.push_back(codec);

        return 0;
    }
#else
    Codec::Codec(LoadFileProc_t _loadFile, SaveFileProc_t _saveFile, GetName_t _getName, GetExtensions_t _getExtensions, HINSTANCE _h_instance)
               : loadFile(_loadFile)
               , saveFile(_saveFile)
               , getName(_getName)
               , getExtensions(_getExtensions)
               , h_instance(_h_instance)
    {
    }

    status_t addCodec(std::string path)
    {

        HINSTANCE h_instance = LoadLibrary(TEXT(path.c_str()));

        if (!h_instance)
        {
            return 1;
        }

        LoadFileProc_t load_file_proc = (LoadFileProc_t)GetProcAddress(h_instance, "loadFile");
        SaveFileProc_t save_file_proc = (SaveFileProc_t)GetProcAddress(h_instance, "saveFile");
        GetName_t get_name_proc = (GetName_t)GetProcAddress(h_instance, "getName");
        GetExtensions_t get_ext_proc = (GetExtensions_t)GetProcAddress(h_instance, "getExtensions");

        if (!load_file_proc || !save_file_proc || !get_name_proc || !get_ext_proc)
        {
            return 2;
        }

        Codec codec(load_file_proc, save_file_proc, get_name_proc, get_ext_proc, h_instance);

        g_codecs.push_back(codec);

        return 0;
    }
#endif
#ifdef __linux__
    status_t initCodecs()
    {
        LOG_F(INFO, "Starting codecs init");

        std::list <std::string> path_to_dll = Utils::searchFilesByExt("codecs\\", ".so");


        for (std::string dll_file : path_to_dll) {

            LOG_F(INFO, "Got file %s", dll_file.c_str());

            if (addCodec(dll_file))
            {
                LOG_F(ERROR, "Cannot load codec .dll %s", dll_file.c_str());
                return 1;
            }
        }

        return 0;
    }
#else
    status_t initCodecs()
    {
        LOG_F(INFO, "Starting codecs init");


        std::list <std::string> path_to_dll = Utils::searchFilesByExt("codecs\\", ".dll");

        for (std::string dll_file : path_to_dll) {

            LOG_F(INFO, "Got file %s", dll_file.c_str());

            if (addCodec(dll_file))
            {
                LOG_F(ERROR, "Cannot load codec .dll %s", dll_file.c_str());
                return 1;
            }
        }

        return 0;
    }
#endif
    const Codec* findCodecByFileExt(std::string ext)
    {
        for (Codec& codec : g_codecs)
        {
            if (ext == codec.getExtensions())
            {
                return &codec;
            }
        }

        return nullptr;
    }

    std::vector<const Codec*> getInitedCodecs()
    {
        std::vector<const Codec*> ret_codecs;

        for (Codec& codec : g_codecs)
        {
            ret_codecs.push_back(&codec);
        }

        return ret_codecs;
    }

    const Codec* getCodecByName(std::string name)
    {
        for (Codec& codec : g_codecs)
        {
            if (codec.getName() == name)
            {
                return &codec;
            }
        }

        return nullptr;
    }

}