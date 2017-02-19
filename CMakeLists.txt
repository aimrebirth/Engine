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
set(CPPAN_HIDE_LOCAL_DEPENDENCIES 1)
set(LIBRARY_TYPE_loc_DataManager_data_manager_local SHARED)
set(LIBRARY_TYPE_loc_DataManager_schema_local SHARED)
add_subdirectory(.cppan)

add_definitions(-DUSE_LOGGER=1)

include_directories(include)
add_subdirectory(src)
