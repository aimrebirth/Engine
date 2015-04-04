#
# Polygon-4
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

################################################################################

########################################
# LIBRARY common
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

set(engine_src
    Engine.cpp
    Mods.cpp
    Test.cpp

    Tools/Logger.cpp
    Tools/Logger.h

    $<TARGET_OBJECTS:hotpatchable>
)

add_library             (Engine SHARED ${engine_src} ${engine_inc})
target_link_libraries   (Engine memory ${Boost_LIBRARIES} dbghelp)
set_output_directory    (Engine ${library_dir})
get_target_property     (name Engine NAME)
set_target_properties   (Engine PROPERTIES PDB_NAME ${name}.${POLYGON4_ARCH})


########################################
# EXECUTABLE Injector
########################################

set(injector_src
    Tools/Injector.cpp
    Tools/Hotpatch.h
)

add_executable          (injector ${injector_src})
target_link_libraries   (injector ${Boost_LIBRARIES})
set_output_directory    (injector ${library_dir})
project_group           (injector Tools)


########################################
# EXECUTABLE Fix PDB
########################################

add_executable          (pdbfix Tools/PdbFix.cpp)
target_link_libraries   (pdbfix dbghelp)
set_output_directory    (pdbfix ${library_dir})
project_group           (pdbfix Tools)

################################################################################