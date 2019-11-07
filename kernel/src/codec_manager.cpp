#include "codec_manager.h"
#include <vector>
#include <windows.h> 
#include <iostream>
#include <filesystem>
#include <list>
#include <string>

namespace CodecManager
{
    static std::vector<CodecInfo> g_codecs;

    std::list <std::string> recursiveDLLSearch(const std::string& search_path, const std::string& extension);

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
        std::list <std::string> path_to_dll = recursiveDLLSearch(".\\", ".dll");

        for (std::string dll_file : path_to_dll) {

            HINSTANCE hinstLib = LoadLibrary(TEXT(dll_file.c_str()));

            if (!hinstLib)
            {
                return 1;
            }

            LoadFileProc_t load_file_proc = (LoadFileProc_t)GetProcAddress(hinstLib, "loadFile");
            SaveFileProc_t save_file_proc = (SaveFileProc_t)GetProcAddress(hinstLib, "saveFile");

            if (!load_file_proc || !save_file_proc)
            {
                return 2;
            }

            g_codecs.push_back(CodecInfo("pure_wave.dll", load_file_proc, save_file_proc));
        }


        return 0;
    }

    std::vector<CodecInfo> getInitedCodecs()
    {
        return g_codecs;
    }

    status_t getCodec(std::string name, CodecInfo& codec_info)
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

    //fun what search files whith need extension
    std::list <std::string> recursiveDLLSearch(const std::string& search_path, const std::string& extension) {

        std::list <std::string> list_of_path_to_dll; // create local list? what contained pathes

        if (std::filesystem::exists(search_path)) {
            for (auto& p : std::filesystem::recursive_directory_iterator(search_path)) {//cycle what run in all folders

                if (!std::filesystem::is_regular_file(p.status()))//check if isn`t directory, if it not file we are continue our cycle
                    continue;

                std::string name = p.path().filename().u8string();//take name of current file
                bool match = !name.compare(name.size() - extension.size(), extension.size(), extension);//check if file has extension what we need

                if (!match)
                    continue;
                list_of_path_to_dll.push_back(p.path().u8string());//write path to file into list
            }
            return list_of_path_to_dll;
        }
        else
            return list_of_path_to_dll;//TODO: check this return




    }
}