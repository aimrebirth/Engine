#pragma once

#include <fstream>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;
using path = fs::path;

#define hotpatch_base_filename  "polygon4.engine.dll"
#define hotpatch_pid_filename   hotpatch_base_filename ".pid"
#define hotpatch_orig_filename  hotpatch_base_filename ".orig"
#define hotpatch_old_filename   hotpatch_base_filename ".old"
#define hotpatch_new_filename   hotpatch_base_filename ".new"
#define hotpatch_ver_filename   hotpatch_base_filename ".ver"

inline path make_temp_filename(const path &filename)
{
    return fs::temp_directory_path() / filename;
}

inline std::string read_file_in_temp(const path &filename)
{
    auto temp = make_temp_filename(filename);
    std::ifstream ifile(temp);
    if (!ifile)
        return "";
    std::string fn;
    std::getline(ifile, fn);
    return fn;
}

inline path read_orig_module_filename()
{
    return read_file_in_temp(hotpatch_orig_filename);
}

inline path read_old_module_filename()
{
    return read_file_in_temp(hotpatch_old_filename);
}

inline path read_new_module_filename()
{
    return read_file_in_temp(hotpatch_new_filename);
}

inline path read_ver_module_filename()
{
    return read_file_in_temp(hotpatch_ver_filename);
}

path &getUe4ModuleName();
void *loadSymbol(const char *symbol);
