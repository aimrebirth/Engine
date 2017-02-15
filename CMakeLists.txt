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

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

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
    endforeach()
endif()

# libraries and dependencies
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
    set(POLYGON4_ARCH "64")
    set(POLYGON4_ARCH_DIR_NAME "64")
    set(POLYGON4_ARCH_FULL "x64")
elseif (SIZEOF_VOID_P EQUAL 4)
    set(X86 1)
    set(POLYGON4_ARCH "86")
    set(POLYGON4_ARCH_DIR_NAME "32")
    set(POLYGON4_ARCH_FULL "x86")
else()
    message(FATAL_ERROR "Cannot determine target architecture")
endif()

################################################################################
#
# compiler & linker settings
#
################################################################################

if (MSVC)
    set(disable_msvc_warnings "/W1 /wd4996")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP ${disable_msvc_warnings}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP ${disable_msvc_warnings}")
endif(MSVC)

################################################################################
#
# build
#
################################################################################

add_subdirectory(.cppan)

include(BuildFunctions)

add_definitions(-DUSE_LOGGER=1)
set(POLYGON4_ENGINE 1)
add_definitions(-DPOLYGON4_ENGINE=1)

if (WIN32)
set(x "__declspec(dllexport)")
add_definitions(-DP4_ENGINE_API=${x})
endif()

include_directories(include)
add_subdirectory(src)
