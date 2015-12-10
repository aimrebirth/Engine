#
# Polygon-4
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

################################################################################

########################################
# CUSTOM BUILD STEP API_lua
########################################

add_custom_command(OUTPUT API_lua.cpp
    COMMAND ${SWIG_EXECUTABLE} -c++ -lua -o API_lua.cpp ${CMAKE_CURRENT_SOURCE_DIR}/API.i
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/API.i
)
add_custom_target(api_lua
    DEPENDS API_lua.cpp
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/API.i
)
project_group           (api_lua API)


########################################
# LIBRARY hotpatchable
########################################

set(hotpatchable_src
    Tools/Hotpatch.cpp
    Tools/Hotpatch.h
    ${CMAKE_SOURCE_DIR}/include/Polygon4/Hotpatch.h
)
add_library             (hotpatchable OBJECT ${hotpatchable_src})
project_group           (hotpatchable Tools)


########################################
# LIBRARY logger
########################################

set(logger_src
    Tools/Logger.cpp
    Tools/Logger.h
)
add_library             (logger ${logger_src})
project_group           (logger Tools)


########################################
# SHARED LIBRARY Engine
########################################

file(GLOB_RECURSE engine_inc "${CMAKE_SOURCE_DIR}/include/Polygon4/*")
file(GLOB engine_src "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set(engine_src
    ${engine_src}
    API.i
    $<TARGET_OBJECTS:hotpatchable>
)

add_library             (Engine SHARED ${engine_src} ${engine_inc})
target_link_libraries   (Engine DataManager logger lua ${Boost_LIBRARIES} dbghelp)
add_dependencies        (Engine api_lua clean_hotpatch)
set_output_directory    (Engine ${library_dir})
get_target_property     (name Engine NAME)
set_target_properties   (Engine PROPERTIES OUTPUT_NAME ${name}.${POLYGON4_ARCH_FULL})
set_target_properties   (Engine PROPERTIES PDB_NAME ${name}.${POLYGON4_ARCH_FULL})
target_compile_definitions(Engine PRIVATE -DPOLYGON4_ENGINE)

add_custom_command(TARGET Engine POST_BUILD
    COMMAND pdbfix $<TARGET_FILE:Engine>
    DEPENDS pdbfix Engine
)

if (WIN32)
    add_custom_command(TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Engine> ${CMAKE_SOURCE_DIR}/../../Binaries/Win${POLYGON4_ARCH_DIR_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:Engine> ${CMAKE_SOURCE_DIR}/../../Binaries/Win${POLYGON4_ARCH_DIR_NAME}/
    )
endif()


########################################
# EXECUTABLE Fix PDB
########################################

add_executable          (pdbfix Tools/PdbFix.cpp)
target_link_libraries   (pdbfix dbghelp)
set_output_directory    (pdbfix ${library_dir})
project_group           (pdbfix Tools)


########################################
# EXECUTABLE Fix project
########################################

add_executable          (fixproject Tools/FixProject.cpp)
set_output_directory    (fixproject ${library_dir})
project_group           (fixproject Tools)


########################################
# CUSTOM BUILD STEP clean_hotpatch
########################################

add_custom_target(clean_hotpatch
    COMMAND fixproject ${CMAKE_CURRENT_BINARY_DIR}/Engine.vcxproj.user ${name}.${POLYGON4_ARCH_FULL} ${library_dir}/${name}.${POLYGON4_ARCH_FULL}
    DEPENDS fixproject
)
project_group           (clean_hotpatch Tools)

################################################################################
