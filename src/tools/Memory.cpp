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

#include <memory>
#include <stdint.h>

void *__default_alloc(size_t size, uint32_t Alignment)
{
    return malloc(size);
}

void __default_free(void *m)
{
    return free(m);
}

typedef void *(__cdecl *__unreal_alloc)(size_t size, uint32_t Alignment);
typedef void  (__cdecl *__unreal_free )(void *m);
typedef void  (__cdecl *InitMemory)(void **, void **);

static __unreal_alloc __alloc = __default_alloc;
static __unreal_free  __free = __default_free;

static const char *InitMemoryName = "InitMemory";

#ifdef WIN32
#include <Windows.h>
#include <psapi.h>

InitMemory find_proc(const char *module)
{
    auto lib = LoadLibrary(module);
    if (!lib)
        return 0;
    void *addr = GetProcAddress(lib, InitMemoryName);
    if (!addr)
        FreeLibrary(lib);
    return (InitMemory)addr;
}

void try_load_mm()
{
    if (__alloc && __free)
        return;

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
                    proc((void **)&__alloc, (void **)&__free);
                    return;
                }
            }
        }
    }
}
#endif

static struct ______init { ______init() { try_load_mm();  } } _____init;

#define OPERATOR_NEW_THROW_SPEC
#define OPERATOR_NEW_NOTHROW_SPEC throw()

#define OPERATOR_DELETE_THROW_SPEC
#define OPERATOR_DELETE_NOTHROW_SPEC throw()

#define OPERATOR_NEW_MSVC_PRAGMA

#define ALIGN 0

#define REPLACEMENT_OPERATOR_NEW_AND_DELETE \
	OPERATOR_NEW_MSVC_PRAGMA void* operator new  ( size_t Size                  ) OPERATOR_NEW_THROW_SPEC      { return ALLOC( Size, ALIGN ); } \
	OPERATOR_NEW_MSVC_PRAGMA void* operator new[]( size_t Size                  ) OPERATOR_NEW_THROW_SPEC      { return ALLOC( Size, ALIGN ); } \
	OPERATOR_NEW_MSVC_PRAGMA void* operator new  ( size_t Size, std::nothrow_t& ) OPERATOR_NEW_NOTHROW_SPEC    { return ALLOC( Size, ALIGN ); } \
	OPERATOR_NEW_MSVC_PRAGMA void* operator new[]( size_t Size, std::nothrow_t& ) OPERATOR_NEW_NOTHROW_SPEC    { return ALLOC( Size, ALIGN ); } \
	void operator delete  ( void* Ptr )                                           OPERATOR_DELETE_THROW_SPEC   { FREE( Ptr ); } \
	void operator delete[]( void* Ptr )                                           OPERATOR_DELETE_THROW_SPEC   { FREE( Ptr ); } \
	void operator delete  ( void* Ptr, std::nothrow_t& )                          OPERATOR_DELETE_NOTHROW_SPEC { FREE( Ptr ); } \
	void operator delete[]( void* Ptr, std::nothrow_t& )                          OPERATOR_DELETE_NOTHROW_SPEC { FREE( Ptr ); }

#define ALLOC __alloc
#define FREE  __free

//REPLACEMENT_OPERATOR_NEW_AND_DELETE