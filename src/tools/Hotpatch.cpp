#include <fstream>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <Windows.h>
#include <Psapi.h>
#include <Dbghelp.h>

#include <boost/filesystem.hpp>

#include <Polygon4/Hotpatch.h>
#include "Hotpatch.h"

#include "Logger.h"
DECLARE_STATIC_LOGGER(logger, "hotpatch");

std::string find_executable_in_path(const std::string &file, std::string path = "");

namespace polygon4
{

String read_orig_module_filename()
{
    return ::read_orig_module_filename();
}

String read_old_module_filename()
{
    return ::read_old_module_filename();
}

String read_new_module_filename()
{
    return ::read_new_module_filename();
}

String read_orig_module_filename_store()
{
    return make_temp_filename(hotpatch_orig_filename);
}

String read_old_module_filename_store()
{
    return make_temp_filename(hotpatch_old_filename);
}

String read_new_module_filename_store()
{
    return make_temp_filename(hotpatch_new_filename);
}

String read_ver_module_filename_store()
{
    return make_temp_filename(hotpatch_ver_filename);
}

std::string convert_time(time_t time)
{
    if (time <= 0)
        return std::string();
    const int sz = 50;
    char buf[sz] = { 0 };
    struct tm *t = localtime(&time);
    strftime(buf, sz, "%d.%m.%Y %H:%M:%S", t);
    if (!*buf)
        return std::string();
    std::string s(buf);
    s = s.c_str();
    return s;
};

void write_module_last_write_time(String game_dir, String module_name)
{
    using namespace boost::filesystem;

    boost::system::error_code ec;

    path base = path(game_dir).normalize();
    path dll = base / "ThirdParty" / module_name / "lib";

    const std::string base_name = "Engine.x64.";
    const std::string ext_dll = "dll";
    const std::string base_dll = base_name + ext_dll;

    auto old_module = canonical(dll / base_dll);

    if (!exists(old_module))
    {
        LOG_DEBUG(logger, "Old module '" << old_module << "' does not exist, writing '-1' as last write time");

        std::ofstream ofile_ver(read_ver_module_filename_store());
        if (ofile_ver)
            ofile_ver << -1;

        return;
    }

    auto lwt = last_write_time(old_module, ec);

    LOG_DEBUG(logger, "last_write_time    : " << convert_time(lwt) << " " << lwt);

    std::ofstream ofile_ver(read_ver_module_filename_store());
    if (ofile_ver)
        ofile_ver << last_write_time(old_module, ec);
}

String prepare_module_for_hotload(String game_dir, String module_name)
{
    using namespace boost::filesystem;

    boost::system::error_code ec;

    path base = path(game_dir).normalize();
    path bin = base / "Binaries" / "Win64";
    path dll = base / "ThirdParty" / module_name / "lib";
    std::string win64dir = "win64";
    path win64 = base / "ThirdParty" / module_name / win64dir;
    path project = win64 / "src" / "Engine.vcxproj.user";
    path pdbfix = dll / "pdbfix.exe";
    path fixproject = dll / "fixproject.exe";
    path result;

    LOG_DEBUG(logger, "Preparing module for hot load: " << module_name.toString());

    const std::string base_name = "Engine.x64.";
    const std::string ext_dll = "dll";
    const std::string ext_pdb = "pdb";
    const std::string base_dll  = base_name + ext_dll;
    const std::string base_pdb  = base_name + ext_pdb;

    auto main_module_dll = canonical(dll / base_dll);
    auto main_module_pdb = canonical(dll / base_pdb);

    if (!exists(main_module_dll))
    {
        LOG_DEBUG(logger, "New module '" << main_module_dll << "' does not exist, cancelling hot load");
        return L"";
    }

    // Check module. If it is not changed, do not reload it.
    LOG_DEBUG(logger, "Checking if module has not changed");
    LOG_DEBUG(logger, "Module: " << main_module_dll.string());

    auto lwt = last_write_time(main_module_dll, ec);
    auto lwt_old = read_ver_module_filename();

    LOG_DEBUG(logger, "last_write_time    : " << convert_time(lwt) << " " << lwt);
    LOG_DEBUG(logger, "last_write_time old: " << convert_time(stoi(lwt_old)) << " " << lwt_old);

    if (ec)
    {
        LOG_DEBUG(logger, "Error occured: " << ec.message());
        return L"";
    }
    if (std::stoi(lwt_old) >= lwt || lwt == -1)
    {
        LOG_DEBUG(logger, "Old module! Nothing to patch...");
        return L"";
    }
    LOG_DEBUG(logger, "We have a new module. Going to patch...");

    auto apply_index = [](std::string file, int i)
    {
        if (i)
            file += std::to_string(i) + ".";
        return file;
    };

    int i = 0;
    while (1)
    {
        int current_i = i++;
        result          = bin / (apply_index(base_name, current_i) + ext_dll);
        path result_pdb = bin / (apply_index(base_name, current_i) + ext_pdb);
        copy_file(main_module_dll, result, copy_option::overwrite_if_exists, ec);
        if (ec)
        {
            LOG_DEBUG(logger, "dst dll is busy: " << result.string());
            continue;
        }
        LOG_DEBUG(logger, "copied: " << result.string());

        std::ofstream ofile_old(read_old_module_filename_store());
        if (ofile_old)
            ofile_old << path(bin / (apply_index(base_name, current_i - 1) + ext_dll)).string();

        std::ofstream ofile_new(read_new_module_filename_store());
        if (ofile_new)
            ofile_new << result.string();

        // fix project: to get new dll with number in name
        //  not fixed dlls cannot take correct pdb
        {
            std::string cmd;
            cmd += "\"" + fixproject.string() + "\"" + " ";
            cmd += "\"" + project.string() + "\"" + " ";
            cmd += "\"" + result.filename().stem().string() + "\"" + " ";
            cmd += "\"" + (dll / result.filename().stem().string()).string() + "\"" + " ";
            LOG_DEBUG(logger, "fix project: " << cmd);
            STARTUPINFO si = { 0 };
            PROCESS_INFORMATION pi = { 0 };
            CreateProcess(fixproject.string().c_str(), (char *)cmd.c_str(), 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &si, &pi);
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD rc = 0;
            GetExitCodeProcess(pi.hProcess, &rc);
            LOG_DEBUG(logger, "fix project rc: " << rc);
        }
        // cmake build: get new dll with number in name
        {
            std::string cmake = find_executable_in_path("cmake");
            std::string cmd;
            cmd += "cmake --build \"" + win64.string() + "\" --config RelWithDebInfo";
            LOG_DEBUG(logger, "cmake exe: " << cmake);
            LOG_DEBUG(logger, "cmake cmd: " << cmd);
            STARTUPINFO si = { 0 };
            PROCESS_INFORMATION pi = { 0 };
            CreateProcess(cmake.c_str(), (char *)cmd.c_str(), 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &si, &pi);
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD rc = 0;
            GetExitCodeProcess(pi.hProcess, &rc);
            LOG_DEBUG(logger, "cmake rc: " << rc);
        }
        // fix pdb: in new dll
        {
            std::string cmd;
            cmd += "\"" + pdbfix.string() + "\"" + " ";
            cmd += "\"" + (dll / result.filename()).string() + "\"" + " ";
            LOG_DEBUG(logger, "pdb fix: " << cmd);
            STARTUPINFO si = { 0 };
            PROCESS_INFORMATION pi = { 0 };
            CreateProcess(pdbfix.string().c_str(), (char *)cmd.c_str(), 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &si, &pi);
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD rc = 0;
            GetExitCodeProcess(pi.hProcess, &rc);
            LOG_DEBUG(logger, "pdb fix rc: " << rc);
        }

        // copy FIXED dll and NEW pdb
        copy_file(dll / (apply_index(base_name, current_i) + ext_dll), result, copy_option::overwrite_if_exists, ec);
        copy_file(dll / (apply_index(base_name, current_i) + ext_pdb), result_pdb, copy_option::overwrite_if_exists, ec);

        // should be after dll fix and copy
        std::ofstream ofile_ver(read_ver_module_filename_store());
        if (ofile_ver)
            ofile_ver << last_write_time(result, ec);

        break;
    }
    i--;

    return result.wstring();
}

}

std::string find_executable_in_path(const std::string &file, std::string path)
{
    BOOST_ASSERT(file.find_first_of("\\/") == std::string::npos);

    std::string result;
    const char *exts[] = { "", ".exe", ".com", ".bat", NULL };
    const char **ext = exts;
    while (*ext)
    {
        char buf[MAX_PATH];
        char *dummy;
        DWORD size = ::SearchPathA(path.empty() ? NULL : path.c_str(), file.c_str(), *ext, MAX_PATH, buf, &dummy);
        BOOST_ASSERT(size < MAX_PATH);
        if (size > 0)
        {
            result = buf;
            break;
        }
        ++ext;
    }
    return result;
}

std::string &getUe4ModuleName()
{
    static std::string ue4_module_name;
    return ue4_module_name;
}

void *loadSymbol(const char *symbol)
{
    LOG_TRACE(logger, "loadSymbol: " << symbol << " from module: " << getUe4ModuleName());
    HMODULE h = LoadLibraryA(getUe4ModuleName().c_str());
    LOG_TRACE(logger, "LoadLibraryA returned: " << h);
    auto s = GetProcAddress(h, symbol);
    LOG_TRACE(logger, "GetProcAddress returned: " << s);
    return s;
}

typedef std::map<void*, void*> Import;
typedef std::map<void*, uint16_t> ExportsOld;
typedef std::map<uint16_t, void*> ExportsNew;

bool WriteToVirtualMemory(void *address, void *value)
{
    bool result = true;
    DWORD size = sizeof(void *);
    DWORD dwOldProtect = 0;
    LOG_TRACE(logger, "Writing " << size << " bytes to " << address << " value " << value);
    if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        if (!WriteProcessMemory(GetCurrentProcess(), address, &value, size, NULL))
        {
            result = false;
            DWORD glr = GetLastError();
            LOG_TRACE(logger, "WriteProcessMemory() is failed. GetLastError(): " << glr);
        }
        DWORD dwOldProtect2 = 0;
        if (!VirtualProtect(address, size, dwOldProtect, &dwOldProtect2))
        {
            result = false;
            DWORD glr = GetLastError();
            LOG_TRACE(logger, "VirtualProtect() (2) is failed. GetLastError(): " << glr);
        }
    }
    else
    {
        result = false;
        DWORD glr = GetLastError();
        LOG_TRACE(logger, "VirtualProtect() (1) is failed. GetLastError(): " << glr);
    }
    return result;
}

Import find_import_table(HMODULE hProgram, PIMAGE_DELAYLOAD_DESCRIPTOR pImportDesc)
{
    Import imports;
    for (; pImportDesc->DllNameRVA; pImportDesc++)
    {
        std::string dll_name = (const char *)hProgram + pImportDesc->DllNameRVA;
        LOG_TRACE(logger, "dll: " << dll_name);

        bool engine = dll_name.find("Engine") != -1;
        bool x64 = dll_name.find(".x64.") != -1;
        bool dll = dll_name.find(".dll") != -1;

        if (engine && x64 && dll)
        {
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hProgram + pImportDesc->ImportAddressTableRVA);
            for (; pThunk->u1.Function; pThunk++)
            {
                void *p = (void*)pThunk->u1.Function;
                imports[p] = &pThunk->u1.Function;
                LOG_TRACE(logger, "Import: function: " << p << ", address: " << imports[p]);
            }

            // set to null, because delayed dll will unload all import on its unload
            // but we want to keep them
            // maybe not needed atm
            //pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hProgram + pImportDesc->UnloadInformationTableRVA);
            //for (; pThunk->u1.Function; pThunk++)
            //    WriteToVirtualMemory(&pThunk->u1.Function, 0);
        }
    }
    return imports;
}

void patch_game_module(const char *module_name, const ExportsOld &exports_orig, const ExportsOld &exports_old, const ExportsNew &exports_new)
{
    DWORD ulSize;
    HMODULE hProgram = GetModuleHandleA(module_name);

    LOG_TRACE(logger, "hProgram: " << hProgram);

    if (!hProgram)
        return;

    PIMAGE_DELAYLOAD_DESCRIPTOR pImportDesc = NULL;
    pImportDesc = (PIMAGE_DELAYLOAD_DESCRIPTOR)ImageDirectoryEntryToData(hProgram, TRUE, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, &ulSize);

    LOG_TRACE(logger, "pImportDesc: " << pImportDesc);

    if (!(pImportDesc))
        return;

    LOG_TRACE(logger, "Imports old:");
    auto imports = find_import_table(hProgram, pImportDesc);

    bool patched = false;

    // PATCH !!!
    // If function is not found, left as before.
    for (const auto &i : imports)
    {
        auto o = exports_old.find(i.first);
        if (o != exports_old.end())
        {
            auto n = exports_new.find(o->second);
            if (n != exports_new.end())
            {
                if (!WriteToVirtualMemory(i.second, n->second))
                {
                    LOG_ERROR(logger, "WriteToVirtualMemory() is failed.");
                }
                else
                {
                    patched = true;
                }
            }
            else
            {
                LOG_TRACE(logger, "New export: " << o->second << " is not found!");
            }
        }
        else
        {
            LOG_TRACE(logger, "Old export: " << i.first << " is not found!");
        }
        o = exports_orig.find(i.first);
        if (o != exports_orig.end())
        {
            auto n = exports_new.find(o->second);
            if (n != exports_new.end())
            {
                if (!WriteToVirtualMemory(i.second, n->second))
                {
                    LOG_ERROR(logger, "WriteToVirtualMemory() is failed.");
                }
                else
                {
                    patched = true;
                }
            }
            else
            {
                LOG_TRACE(logger, "New export: " << o->second << " is not found!");
            }
        }
        else
        {
            LOG_TRACE(logger, "Orig export: " << i.first << " is not found!");
        }
    }

    if (patched)
    {
        getUe4ModuleName() = module_name;
    }

    LOG_TRACE(logger, "Imports new:");
    imports = find_import_table(hProgram, pImportDesc);
}

void patch_all_game_modules(const ExportsOld &exports_orig, const ExportsOld &exports_old, const ExportsNew &exports_new)
{
    HMODULE hMods[1024];
    auto hProcess = GetCurrentProcess();
    DWORD cbNeeded;

    LOG_DEBUG(logger, "Enumerating loaded modules");

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
                std::string module_name = szModName;
                module_name = module_name.c_str();

                LOG_TRACE(logger, "Checking module: " << module_name);

                bool ue4editor = module_name.find("UE4Editor") != -1;
                bool polygon4  = module_name.find("Polygon4") != -1;
                if (ue4editor && polygon4)
                {
                    LOG_DEBUG(logger, "Patching module: " << module_name);

                    patch_game_module(module_name.c_str(), exports_orig, exports_old, exports_new);
                }
            }
        }
    }
}

void patch_import_table()
{
    DWORD ulSize;

    const std::string orig_dll = boost::filesystem::path(read_orig_module_filename()).filename().string();
    const std::string old_dll = boost::filesystem::path(read_old_module_filename()).filename().string();
    const std::string new_dll = boost::filesystem::path(read_new_module_filename()).filename().string();

    LOG_TRACE(logger, "orig_dll: " << orig_dll);
    LOG_TRACE(logger, "old_dll: " << old_dll);
    LOG_TRACE(logger, "new_dll: " << new_dll);

    HMODULE hOrig = GetModuleHandle(orig_dll.c_str());
    HMODULE hOld = GetModuleHandle(old_dll.c_str());
    HMODULE hNew = GetModuleHandle(new_dll.c_str());

    LOG_TRACE(logger, "hOrig: " << hOrig);
    LOG_TRACE(logger, "hOld: " << hOld);
    LOG_TRACE(logger, "hNew: " << hNew);

    if (!(hOrig && hOld && hNew))
        return;

    PIMAGE_EXPORT_DIRECTORY pExportDescOrig = NULL;
	pExportDescOrig = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(hOrig, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);

    PIMAGE_EXPORT_DIRECTORY pExportDescOld = NULL;
	pExportDescOld = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(hOld, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);

    PIMAGE_EXPORT_DIRECTORY pExportDescNew = NULL;
	pExportDescNew = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(hNew, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);

    LOG_TRACE(logger, "pExportDescOrig: " << pExportDescOrig);
    LOG_TRACE(logger, "pExportDescOld: " << pExportDescOld);
    LOG_TRACE(logger, "pExportDescNew: " << pExportDescNew);

    if (!(pExportDescOrig && pExportDescOld && pExportDescNew))
        return;

    LOG_TRACE(logger, "Exports orig:");
    ExportsOld exports_orig;
    for (size_t i = 1; i <= pExportDescOrig->NumberOfFunctions; i++)
    {
        auto address = GetProcAddress(hOrig, (LPCSTR)i);
        exports_orig[address] = i;
        LOG_TRACE(logger, "Export: address: " << address);
    }

    LOG_TRACE(logger, "Exports old:");
    ExportsOld exports_old;
    for (size_t i = 1; i <= pExportDescOld->NumberOfFunctions; i++)
    {
        auto address = GetProcAddress(hOld, (LPCSTR)i);
        exports_old[address] = i;
        LOG_TRACE(logger, "Export: address: " << address);
    }

    LOG_TRACE(logger, "Exports new:");
    ExportsNew exports_new;
    for (size_t i = 1; i <= pExportDescNew->NumberOfFunctions; i++)
    {
        auto address = GetProcAddress(hNew, (LPCSTR)i);
        exports_new[i] = address;
        LOG_TRACE(logger, "Export: address: " << address);
    }

    patch_all_game_modules(exports_orig, exports_old, exports_new);
}

bool same_pid()
{
    DWORD pid = GetCurrentProcessId();
    DWORD pid2 = 0;
    auto temp = make_temp_filename(hotpatch_pid_filename);
    LOG_TRACE(logger, "PID file: " << temp);
    std::ifstream ifile(temp);
    if (ifile)
    {
        LOG_TRACE(logger, "Current PID: " << pid);
        ifile >> pid2;
        LOG_TRACE(logger, "File PID: " << pid2);
        if (pid == pid2)
            return true;
        ifile.close();
    }
    std::ofstream ofile(temp);
    if (!ofile)
        return false;
    ofile << pid;
    return false;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            char filename[MAX_PATH] = { 0 };
            GetModuleFileName(hinstDLL, filename, MAX_PATH);
            auto p = boost::filesystem::path(filename);
            p = p.parent_path() / p.stem();
            LOGGER_CONFIGURE("Debug", p.string());

            LOG_TRACE(logger, "Checking PID...");
            bool pid = same_pid();
            if (pid)
            {
                LOG_TRACE(logger, "Already exists!");
                LOG_DEBUG(logger, "Patching imports...");
                patch_import_table();
            }
            else
            {
                LOG_TRACE(logger, "PID is written");
            }
        }
		break;
	}
    return TRUE;
}
