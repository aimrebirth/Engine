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

namespace polygon4
{

std::string read_orig_module_filename()
{
    return ::read_orig_module_filename();
}

std::string read_old_module_filename()
{
    return ::read_old_module_filename();
}

std::string read_new_module_filename()
{
    return ::read_new_module_filename();
}

std::string read_orig_module_filename_store()
{
    return boost::filesystem::temp_directory_path().string() + hotpatch_orig_filename;
}

std::string read_old_module_filename_store()
{
    return boost::filesystem::temp_directory_path().string() + hotpatch_old_filename;
}

std::string read_new_module_filename_store()
{
    return boost::filesystem::temp_directory_path().string() + hotpatch_new_filename;
}

std::string read_ver_module_filename_store()
{
    return boost::filesystem::temp_directory_path().string() + hotpatch_ver_filename;
}

std::wstring prepare_module_for_hotload(std::wstring game_dir, std::string module_name)
{
    using namespace boost::filesystem;
    
    boost::system::error_code ec;

    path base = path(game_dir).normalize();
    path bin = base / "Binaries" / "Win64";
    path dll = base / "ThirdParty" / module_name / "lib";
    path result;

    LOG_DEBUG(logger, "Preparing module for hot load: " << module_name);

    const std::string base_name = "Engine.x64.";
    const std::string ext_dll = "dll";
    const std::string ext_pdb = "pdb";
    const std::string base_dll  = base_name + ext_dll;
    const std::string base_pdb  = base_name + ext_pdb;

    if (!exists(dll / base_dll))
    {
        LOG_DEBUG(logger, "New module does not exist, cancelling hot load");
        return std::wstring();
    }

    // Check module. If it is not changed, do not reload it.
    LOG_DEBUG(logger, "Checking if module has not changed");
    LOG_DEBUG(logger, "Module: " << path(dll / base_dll).string());
    auto lwt = last_write_time(dll / base_dll, ec);
    LOG_DEBUG(logger, "last_write_time: " << lwt);
    if (ec)
    {
        LOG_DEBUG(logger, "Error occured: " << ec.message());
        return std::wstring();
    }
    if (read_ver_module_filename() == std::to_string(lwt) || lwt == -1)
    {
        LOG_DEBUG(logger, "Old module! Nothing to patch...");
        return std::wstring();
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
        result = bin / (apply_index(base_name, current_i) + ext_dll);
        copy_file(dll / base_dll, result, copy_option::overwrite_if_exists, ec);
        if (ec)
        {
            LOG_DEBUG(logger, "dst dll is busy: " << result.string());
            continue;
        }
        LOG_DEBUG(logger, "copied: " << result.string());
        copy_file(dll / base_pdb, bin / (apply_index(base_name, current_i) + ext_pdb), copy_option::overwrite_if_exists, ec);

        std::ofstream ofile_old(read_old_module_filename_store());
        if (ofile_old)
            ofile_old << path(bin / (apply_index(base_name, current_i - 1) + ext_dll)).string();

        std::ofstream ofile_new(read_new_module_filename_store());
        if (ofile_new)
            ofile_new << result.string();

        std::ofstream ofile_ver(read_ver_module_filename_store());
        if (ofile_ver)
            ofile_ver << last_write_time(result, ec);

        break;
    }
    i--;

    return result.wstring();
}

}

typedef std::map<void*, void*> Import;
typedef std::map<void*, uint16_t> ExportsOld;
typedef std::map<uint16_t, void*> ExportsNew;

Import find_import_table(HMODULE hProgram, PIMAGE_IMPORT_DESCRIPTOR pImportDesc)
{
    Import imports;
    for (; pImportDesc->Name; pImportDesc++)
    {
        std::string dll_name = (const char *)hProgram + pImportDesc->Name;
        LOG_DEBUG(logger, "dll: " << dll_name);

        bool engine = dll_name.find("Engine") != -1;
        bool x64 = dll_name.find(".x64.") != -1;
        bool dll = dll_name.find(".dll") != -1;

        if (engine && x64 && dll)
        {
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hProgram + pImportDesc->FirstThunk);
            for (; pThunk->u1.Function; pThunk++)
            {
                void *p = (void*)pThunk->u1.Function;
                imports[p] = &pThunk->u1.Function;
                LOG_DEBUG(logger, "Import: function: " << p << ", address: " << imports[p]);
            }
            break;
        }
    }
    return imports;
}

bool WriteToVirtualMemory(void *address, void *value)
{
    bool result = true;
    DWORD size = sizeof(void *);
	DWORD dwOldProtect = 0;
    LOG_DEBUG(logger, "Trying to write " << size << " bytes to " << address << " value " << value);
	if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &dwOldProtect)) 
	{
        if (!WriteProcessMemory(GetCurrentProcess(), address, &value, size, NULL))
        {
            result = false;
            DWORD glr = GetLastError();
            LOG_DEBUG(logger, "WriteProcessMemory() is failed. GetLastError(): " << glr);
        }
        if (!VirtualProtect(address, size, dwOldProtect, &dwOldProtect))
        {
            result = false;
            DWORD glr = GetLastError();
            LOG_DEBUG(logger, "VirtualProtect() (2) is failed. GetLastError(): " << glr);
        }
	}
    else
    {
        result = false;
        DWORD glr = GetLastError();
        LOG_DEBUG(logger, "VirtualProtect() (1) is failed. GetLastError(): " << glr);
    }
    return result;
}

void patch_game_module(const char *module_name, const ExportsOld &exports_orig, const ExportsOld &exports_old, const ExportsNew &exports_new)
{
    DWORD ulSize;
    HMODULE hProgram = GetModuleHandleA(module_name);

    LOG_DEBUG(logger, "hProgram: " << hProgram);

    if (!hProgram)
        return;
    
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hProgram, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

    LOG_DEBUG(logger, "pImportDesc: " << pImportDesc);

    if (!(pImportDesc))
        return;
    
    LOG_DEBUG(logger, "Imports old:");
    auto imports = find_import_table(hProgram, pImportDesc);

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
                    LOG_DEBUG(logger, "WriteToVirtualMemory() is failed.");
                }
            }
            else
            {
                LOG_DEBUG(logger, "New export: " << o->second << " is not found!");
            }
        }
        else
        {
            LOG_DEBUG(logger, "Old export: " << i.first << " is not found!");
        }
        o = exports_orig.find(i.first);
        if (o != exports_orig.end())
        {
            auto n = exports_new.find(o->second);
            if (n != exports_new.end())
            {
                if (!WriteToVirtualMemory(i.second, n->second))
                {
                    LOG_DEBUG(logger, "WriteToVirtualMemory() is failed.");
                }
            }
            else
            {
                LOG_DEBUG(logger, "New export: " << o->second << " is not found!");
            }
        }
        else
        {
            LOG_DEBUG(logger, "Orig export: " << i.first << " is not found!");
        }
    }
    
    LOG_DEBUG(logger, "Imports new:");
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

                LOG_DEBUG(logger, "Checking module: " << module_name);

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
    
    const std::string orig_dll  = boost::filesystem::path(read_orig_module_filename()).filename().string();
    const std::string old_dll  = boost::filesystem::path(read_old_module_filename()).filename().string();
    const std::string new_dll  = boost::filesystem::path(read_new_module_filename()).filename().string();
    
    LOG_DEBUG(logger, "orig_dll: " << orig_dll);
    LOG_DEBUG(logger, "old_dll: " << old_dll);
    LOG_DEBUG(logger, "new_dll: " << new_dll);
    
    HMODULE hOrig = GetModuleHandle(orig_dll.c_str());
    HMODULE hOld = GetModuleHandle(old_dll.c_str());
    HMODULE hNew = GetModuleHandle(new_dll.c_str());
    
    LOG_DEBUG(logger, "hOrig: " << hOrig);
    LOG_DEBUG(logger, "hOld: " << hOld);
    LOG_DEBUG(logger, "hNew: " << hNew);

    if (!(hOrig && hOld && hNew))
        return;

    PIMAGE_EXPORT_DIRECTORY pExportDescOrig = NULL;
	pExportDescOrig = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(hOrig, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);

    PIMAGE_EXPORT_DIRECTORY pExportDescOld = NULL;
	pExportDescOld = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(hOld, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);

    PIMAGE_EXPORT_DIRECTORY pExportDescNew = NULL;
	pExportDescNew = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(hNew, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);
    
    LOG_DEBUG(logger, "pExportDescOrig: " << pExportDescOrig);
    LOG_DEBUG(logger, "pExportDescOld: " << pExportDescOld);
    LOG_DEBUG(logger, "pExportDescNew: " << pExportDescNew);

    if (!(pExportDescOrig && pExportDescOld && pExportDescNew))
        return;

    LOG_DEBUG(logger, "Exports orig:");
    ExportsOld exports_orig;
    for (int i = 1; i <= pExportDescOrig->NumberOfFunctions; i++)
    {
        auto address = GetProcAddress(hOrig, (LPCSTR)i);
        exports_orig[address] = i;
        LOG_DEBUG(logger, "Export: address: " << address);
    }

    LOG_DEBUG(logger, "Exports old:");
    ExportsOld exports_old;
    for (int i = 1; i <= pExportDescOld->NumberOfFunctions; i++)
    {
        auto address = GetProcAddress(hOld, (LPCSTR)i);
        exports_old[address] = i;
        LOG_DEBUG(logger, "Export: address: " << address);
    }

    LOG_DEBUG(logger, "Exports new:");
    ExportsNew exports_new;
    for (int i = 1; i <= pExportDescNew->NumberOfFunctions; i++)
    {
        auto address = GetProcAddress(hNew, (LPCSTR)i);
        exports_new[i] = address;
        LOG_DEBUG(logger, "Export: address: " << address);
    }

    patch_all_game_modules(exports_orig, exports_old, exports_new);
}

bool same_pid()
{
    DWORD pid = GetCurrentProcessId();
    DWORD pid2 = 0;
    auto temp = boost::filesystem::temp_directory_path().string() + hotpatch_pid_filename;
    LOG_DEBUG(logger, "PID file: " << temp);
    std::ifstream ifile(temp);
    if (ifile)
    {
        LOG_DEBUG(logger, "Current PID: " << pid);
        ifile >> pid2;
        LOG_DEBUG(logger, "File PID: " << pid2);
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

            LOG_DEBUG(logger, "Checking PID...");
            if (same_pid())
            {
                LOG_DEBUG(logger, "Already exists!");
                LOG_DEBUG(logger, "Patching imports...");
                patch_import_table();
            }
            else
            {
                LOG_DEBUG(logger, "PID is written");
            }
        }
		break;
	}
    return TRUE;
}

