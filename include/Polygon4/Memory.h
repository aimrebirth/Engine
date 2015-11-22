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

#pragma once

#include <memory>
#include <stdint.h>

#include <Polygon4/DataManager/Dll.h>

#ifdef WIN32
#define POLYGON4_UNREAL_MEMORY_STUB \
    static void* dll_alloc(size_t size) \
    { \
        return FMemory::Malloc(size); \
    } \
    \
    static void dll_free(void *ptr) \
    { \
        FMemory::Free(ptr); \
    } \
    \
    extern "C" __declspec(dllexport) \
    void __cdecl Polygon4InitMemory(void **alloc, void **free) \
    { \
        *alloc = (void*)&dll_alloc; \
        *free = (void*)&dll_free; \
    }

#include <Windows.h>

extern "C"
HRESULT WINAPI __HrLoadAllImportsForDll(LPCSTR szDll);
extern "C"
BOOL WINAPI __FUnloadDelayLoadedDLL2(LPCSTR szDll);

#else // !WIN32

#define POLYGON4_UNREAL_MEMORY_STUB

#endif

inline void Polygon4InitMm(const char *dll)
{
#if WIN32
    __HrLoadAllImportsForDll(dll);
#endif
}

inline void Polygon4ResetMm(const char *dll)
{
#if WIN32
    __FUnloadDelayLoadedDLL2(dll);
#endif
}
