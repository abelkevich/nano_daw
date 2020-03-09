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

    Codec::Codec(LoadFileProc_t _loadFile, SaveFileProc_t _saveFile, GetName_t _getName, GetExtensions_t _getExtensions, void* _h_instance)
               : loadFile(_loadFile)
               , saveFile(_saveFile)
               , getName(_getName)
               , getExtensions(_getExtensions)
               , h_instance(_h_instance)
    {
    }

    static bool addCodec(const std::string &path)
    {
        LOG_F(INFO, "Adding codec lib: '%s'", path.c_str());

        void* h_instance = Utils::loadLibrary(path);

        if (!h_instance)
        {
            LOG_F(ERROR, "Cannot get system descriptor for lib: '%s'", path.c_str());
            return false;
        }

        LOG_F(INFO, "Got instance '%p' for codec lib: '%s'", h_instance, path.c_str());

        LoadFileProc_t load_file_proc = (LoadFileProc_t) Utils::getLibProcedure(h_instance, "loadFile");

        if (!load_file_proc)
        {
            LOG_F(ERROR, "Cannot get 'loadFile' procedure (lib: '%s')", path.c_str());
            return false;
        }

        SaveFileProc_t save_file_proc = (SaveFileProc_t) Utils::getLibProcedure(h_instance, "saveFile");

        if (!save_file_proc)
        {
            LOG_F(ERROR, "Cannot get 'saveFile' procedure (lib: '%s')", path.c_str());
            return false;
        }

        GetName_t get_name_proc = (GetName_t) Utils::getLibProcedure(h_instance, "getName");

        if (!get_name_proc)
        {
            LOG_F(ERROR, "Cannot get 'getName' procedure (lib: '%s')", path.c_str());
            return false;
        }

        GetExtensions_t get_ext_proc = (GetExtensions_t) Utils::getLibProcedure(h_instance, "getExtensions");

        if (!get_ext_proc)
        {
            LOG_F(ERROR, "Cannot get 'getExtensions' procedure (lib: '%s')", path.c_str());
            return false;
        }
        
        LOG_F(INFO, "Codec lib (path: '%s' instance: '%p') was added to the inited codecs", path.c_str(), h_instance);

        Codec codec(load_file_proc, save_file_proc, get_name_proc, get_ext_proc, h_instance);
        g_codecs.push_back(codec);

        return 0;
    }

    bool initCodecs()
    {
        LOG_F(INFO, "Starting codecs init");

        std::list <std::string> libs_paths = Utils::searchFilesByExt("codecs", Utils::c_lib_extension);
        LOG_F(INFO, "Found '%d' possible codecs libs", libs_paths.size());

        for (const std::string& path : libs_paths) 
        {
            if (!addCodec(path))
            {
                LOG_F(ERROR, "Cannot load lib: '%s'", path.c_str());
                return false;
            }
        }

        return true;
    }

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