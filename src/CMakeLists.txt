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
# LIBRARY memory
########################################

add_library             (memory Tools/Memory.cpp)
project_group           (memory Tools)


########################################
# LIBRARY hotpatchable
########################################

set(hotpatchable_src
    Tools/Hotpatch.cpp
    Tools/Hotpatch.h
    ${CMAKE_SOURCE_DIR}/include/polygon4/Hotpatch.h
)
add_library             (hotpatchable OBJECT ${hotpatchable_src})
project_group           (hotpatchable Tools)


########################################
# SHARED LIBRARY Engine
########################################

file(GLOB engine_inc "${CMAKE_SOURCE_DIR}/include/polygon4/*")
file(GLOB engine_src "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set(engine_src
    ${engine_src}

    API.i

    Tools/Logger.cpp
    Tools/Logger.h

    $<TARGET_OBJECTS:hotpatchable>
)

add_library             (Engine SHARED ${engine_src} ${engine_inc})
target_link_libraries   (Engine memory lua sqlite3 ${Boost_LIBRARIES} dbghelp)
add_dependencies        (Engine api_lua)
set_output_directory    (Engine ${library_dir})
get_target_property     (name Engine NAME)
set_target_properties   (Engine PROPERTIES PDB_NAME ${name}.${POLYGON4_ARCH})


########################################
# EXECUTABLE Fix PDB
########################################

#add_executable          (pdbfix Tools/PdbFix.cpp)
#target_link_libraries   (pdbfix dbghelp)
#set_output_directory    (pdbfix ${library_dir})
#project_group           (pdbfix Tools)

################################################################################