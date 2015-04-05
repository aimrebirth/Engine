#pragma once

#include <string>

#include <boost/filesystem.hpp>

#define hotpatch_base_filename  "polygon4.engine.dll"
#define hotpatch_pid_filename   hotpatch_base_filename ".pid"
#define hotpatch_orig_filename  hotpatch_base_filename ".orig"
#define hotpatch_old_filename   hotpatch_base_filename ".old"
#define hotpatch_new_filename   hotpatch_base_filename ".new"
#define hotpatch_ver_filename   hotpatch_base_filename ".ver"

inline std::string read_file_in_temp(const std::string &filename)
{
    auto temp = boost::filesystem::temp_directory_path().string() + filename;
    std::ifstream ifile(temp);
    if (!ifile)
        return "";
    std::string fn;
    std::getline(ifile, fn);
    return fn;
}

inline std::string read_orig_module_filename()
{
    return read_file_in_temp(hotpatch_orig_filename);
}

inline std::string read_old_module_filename()
{
    return read_file_in_temp(hotpatch_old_filename);
}

inline std::string read_new_module_filename()
{
    return read_file_in_temp(hotpatch_new_filename);
}

inline std::string read_ver_module_filename()
{
    return read_file_in_temp(hotpatch_ver_filename);
}
