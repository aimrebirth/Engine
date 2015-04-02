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

set(output_dir  ${CMAKE_BINARY_DIR}/bin)
set(library_dir ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_dir})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_dir})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_dir})

################################################################################
#
# cmake settings
#
################################################################################

project(engine C CXX)

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
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(Boost_USE_DEBUG_RUNTIME OFF)
else()
    ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
endif(WIN32)

find_package(Boost 1.54 REQUIRED filesystem)

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

set(CMAKE_SHARED_MODULE_SUFFIX .${POLYGON4_ARCH}${CMAKE_SHARED_MODULE_SUFFIX})
set(CMAKE_STATIC_LIBRARY_SUFFIX .${POLYGON4_ARCH}${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CMAKE_SHARED_LIBRARY_SUFFIX .${POLYGON4_ARCH}${CMAKE_SHARED_LIBRARY_SUFFIX})
set(CMAKE_IMPORT_LIBRARY_SUFFIX .${POLYGON4_ARCH}${CMAKE_IMPORT_LIBRARY_SUFFIX})

################################################################################
#
# build
#
################################################################################

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)

add_subdirectory(src)