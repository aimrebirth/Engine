#
# Polygon-4
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

################################################################################

########################################
# LIBRARY logger
########################################

set(logger_src
    Tools/Logger.cpp
    Tools/Logger.h
)
add_library             (logger ${logger_src})
target_link_libraries   (logger pvt.cppan.demo.boost.log)
project_group           (logger Tools)


########################################
# SHARED LIBRARY Engine
########################################

set(api_name ScriptAPI)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp
#    COMMAND ${SWIG_EXECUTABLE} -c++ -lua -o ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${api_name}.i
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${api_name}.i
#)
add_custom_target(${api_name}_lua
    COMMAND ${SWIG_EXECUTABLE} -c++ -lua -o ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${api_name}.i
    COMMENT "Generating ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp"
)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp PROPERTIES
    HEADER_FILE_ONLY True
    GENERATED True
)

file(GLOB_RECURSE engine_inc "${CMAKE_SOURCE_DIR}/include/Polygon4/*")
file(GLOB engine_src "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set(engine_src
    ${engine_src}
    ${api_name}.i
    ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp

    Tools/Hotpatch.cpp
    Tools/Hotpatch.h
    ${CMAKE_SOURCE_DIR}/include/Polygon4/Hotpatch.h
)

add_library             (Engine SHARED ${engine_src} ${engine_inc})
target_link_libraries   (Engine logger data_manager dbghelp
    pvt.cppan.demo.boost.filesystem
    pvt.cppan.demo.lua
)
add_dependencies        (Engine clean_hotpatch ${api_name}_lua)
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

# generate fiels for UE4
file(GENERATE
    OUTPUT ${PROJECT_BINARY_DIR}/includes.txt
    CONTENT $<TARGET_PROPERTY:Engine,INTERFACE_INCLUDE_DIRECTORIES>
)
file(GENERATE
    OUTPUT ${PROJECT_BINARY_DIR}/data_manager.txt
    CONTENT $<TARGET_FILE:data_manager>
)
file(GENERATE
    OUTPUT ${PROJECT_BINARY_DIR}/schema.txt
    CONTENT $<TARGET_FILE:schema>
)


########################################
# EXECUTABLE Fix PDB
########################################

add_executable          (pdbfix Tools/PdbFix.cpp)
target_link_libraries   (pdbfix dbghelp pvt.cppan.demo.boost.filesystem)
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
