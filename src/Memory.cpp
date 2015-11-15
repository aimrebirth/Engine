/*
* Polygon-4 Engine
* Copyright (C) 2015 lzwdgc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Polygon4/Memory.h>

#ifdef WIN32

auto dll_alloc = &malloc;
auto dll_free = &free;

void Polygon4InitMm(decltype(malloc) alloc, decltype(free) free)
{
    dll_alloc = alloc;
    dll_free = free;
}

void Polygon4ResetMm()
{
    dll_alloc = &malloc;
    dll_free = &free;
}

void* operator new  (size_t size){ return dll_alloc(size); }
void* operator new[](size_t size) { return dll_alloc(size); }
void* operator new  (size_t size, const std::nothrow_t&) throw(){ return dll_alloc(size); }
void* operator new[](size_t size, const std::nothrow_t&) throw() { return dll_alloc(size); }
void operator delete  (void* ptr) { dll_free(ptr); }
void operator delete[](void* ptr) { dll_free(ptr); }
void operator delete  (void* ptr, const std::nothrow_t&) throw() { dll_free(ptr); }
void operator delete[](void* ptr, const std::nothrow_t&) throw() { dll_free(ptr); }

#define POLYGON4_INIT_MEMORY_FUNCTION Polygon4InitMemory
#define POLYGON4_INIT_MEMORY_FUNCTION_NAME "Polygon4InitMemory"

using POLYGON4_INIT_MEMORY_FUNCTION = void(__cdecl *)(void **, void **);

static const char *InitMemoryName = POLYGON4_INIT_MEMORY_FUNCTION_NAME;

#include <Windows.h>
#include <psapi.h>

POLYGON4_INIT_MEMORY_FUNCTION find_proc(const char *module)
{
    auto lib = LoadLibrary(module);
    if (!lib)
        return 0;
    return (POLYGON4_INIT_MEMORY_FUNCTION)GetProcAddress(lib, InitMemoryName);
}

void try_load_mm()
{
    HMODULE hMods[1024];
    auto hProcess = GetCurrentProcess();
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
                auto proc = find_proc(szModName);
                if (proc)
                {
                    proc((void **)&dll_alloc, (void **)&dll_free);
                    return;
                }
            }
        }
    }
}

/*static
struct ______init
{
    ______init()
    {
        try_load_mm();
    }
} ______init;*/

#endif
