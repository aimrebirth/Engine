#
# Polygon-4 Engine
#

################################################################################
#
# cmake settings
#
################################################################################

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(output_dir ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_dir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_dir})

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

################################################################################
#
# cmake settings
#
################################################################################

project(Engine C CXX)

# libraries and dependencies
find_package(SWIG 3.0 REQUIRED)

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

set(CPPAN_USE_CACHE 0)
set(CPPAN_BUILD_SHARED_LIBS 0)
#set(CPPAN_HIDE_LOCAL_DEPENDENCIES 1)
set(LIBRARY_TYPE_pvt_lzwdgc_polygon4_data_manager_data_manager_master SHARED)
set(LIBRARY_TYPE_pvt_lzwdgc_polygon4_data_manager_schema_master SHARED)
add_definitions(-DDATA_MANAGER_ALIGNED_ALLOCATOR=1)
#set(LIBRARY_TYPE_pvt_cppan_demo_intel_tbb_tbbmalloc_2018_7_18 STATIC)
#set(LIBRARY_TYPE_pvt_cppan_demo_intel_tbb_tbbmalloc_proxy_2018_7_18 STATIC)
add_subdirectory(.cppan)

add_definitions(-DUSE_LOGGER=1)

include_directories(include)
add_subdirectory(src)
