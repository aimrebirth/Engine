#
# Polygon-4 Core Modules
#
#  This file has following organization:
#  1. cmake settings
#  2. project settings
#  3. configure
#  4. compiler & linker settings
#  5. build
#

################################################################################
#
# cmake settings
#
################################################################################

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(output_dir  ${CMAKE_BINARY_DIR}/bin)
set(library_dir ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_dir})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_dir})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_dir})

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

################################################################################
#
# cmake settings
#
################################################################################

project(Engine C CXX)

if (MSVC OR XCODE)
    foreach(conf ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${conf} conf2)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${conf2} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${conf2} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
        #set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${conf2} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    endforeach()
endif()

# libraries and dependencies
if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
else()
    ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
endif(WIN32)

find_package(Boost 1.54 REQUIRED filesystem log)
find_package(SWIG 3.0 REQUIRED)

################################################################################
#
# configure
#
################################################################################

include(CheckTypeSize)
check_type_size("void *" SIZEOF_VOID_P)

if (SIZEOF_VOID_P EQUAL 8)
    set(X64 1)
    set(POLYGON4_ARCH "x64")
elseif (SIZEOF_VOID_P EQUAL 4)
    set(X86 1)
    set(POLYGON4_ARCH "x86")
else()
    message(FATAL_ERROR "Cannot determine target architecture")
endif()

################################################################################
#
# compiler & linker settings
#
################################################################################

if (MSVC)
    add_definitions(-DITERATOR_DEBUG_LEVEL=0)

    set(disable_msvc_warnings "/W1 /wd4996")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP ${disable_msvc_warnings}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP ${disable_msvc_warnings}")
endif(MSVC)

################################################################################
#
# build
#
################################################################################

include(BuildFunctions)

add_definitions(-DUSE_LOGGER)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)
include_directories(dep)
include_directories(dep/dbmgr/include)

set(p dep/lua)
set(lua_src
    ${p}/lapi.c ${p}/lcode.c ${p}/lctype.c ${p}/ldebug.c ${p}/ldo.c ${p}/ldump.c ${p}/lfunc.c ${p}/lgc.c ${p}/llex.c
    ${p}/lmem.c ${p}/lobject.c ${p}/lopcodes.c ${p}/lparser.c ${p}/lstate.c ${p}/lstring.c ${p}/ltable.c
    ${p}/ltm.c ${p}/lundump.c ${p}/lvm.c ${p}/lzio.c
    ${p}/lauxlib.c ${p}/lbaselib.c ${p}/lbitlib.c ${p}/lcorolib.c ${p}/ldblib.c ${p}/liolib.c
	${p}/lmathlib.c ${p}/loslib.c ${p}/lstrlib.c ${p}/ltablib.c ${p}/lutf8lib.c ${p}/loadlib.c ${p}/linit.c
)
unset(p)
include_directories(dep/lua/)

add_library             (lua ${lua_src})
project_group           (lua Extern)

add_subdirectory(dep/dbmgr)
project_group           (sqlite3 Extern)
project_group           (DatabaseManager Extern)

add_subdirectory(src)