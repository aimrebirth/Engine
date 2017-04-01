#
# Polygon-4
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

################################################################################

#######################################
# FUNCTION project_group
#######################################

function(project_group target name)
    set_target_properties(${target} PROPERTIES FOLDER ${name})
endfunction(project_group)


########################################
# LIBRARY logger
########################################

set(logger_src
    Tools/Logger.cpp
    Tools/Logger.h
)
add_library             (logger ${logger_src})
target_link_libraries   (logger pvt.cppan.demo.boost.log)
project_group           (logger tools)


########################################
# SHARED LIBRARY Engine
########################################

set(api_name ScriptAPI)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp
#    COMMAND ${SWIG_EXECUTABLE} -c++ -lua -o ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${api_name}.i
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${api_name}.i
#)
add_custom_target(${api_name}_lua
    COMMAND ${SWIG_EXECUTABLE} -c++ -lua -o ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp2 ${CMAKE_CURRENT_SOURCE_DIR}/${api_name}.i
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp2 ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp
    COMMENT "Generating ${CMAKE_CURRENT_BINARY_DIR}/${api_name}_lua.cpp"
)
project_group(${api_name}_lua misc)
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



function(set_target_property target prop val)
    get_target_property     (a ${target} ALIASED_TARGET)
    if (${a} STREQUAL "a-NOTFOUND")
    set_target_properties   (${target} PROPERTIES ${prop} "${val}")
    else()
    set_target_properties   (${a} PROPERTIES ${prop} "${val}")
    endif()
endfunction()

function(set_target_cd target def)
    get_target_property     (a ${target} ALIASED_TARGET)
    if (${a} STREQUAL "a-NOTFOUND")
    target_compile_definitions(${target} PRIVATE ${def}=)
    else()
    target_compile_definitions(${a} PRIVATE ${def}=)
    endif()
endfunction()

function(remove_defs target def)
    get_target_property(v ${target} INTERFACE_COMPILE_DEFINITIONS)
    #message("${v}")
    set(v2)
    foreach(i ${v})
        string(FIND "${i}" "${def}" p)
        if (NOT ${p} EQUAL 0)
            set(v2 ${v2} ${i})
        endif()
    endforeach()
    #message("${v2}")
    set_target_property(${target} INTERFACE_COMPILE_DEFINITIONS "${v2}")

    set_target_cd(generator ${def})
    set_target_cd(data_manager ${def})
    set_target_cd(schema ${def})
endfunction()

#remove_defs(data_manager "DATA_MANAGER_API")
#remove_defs(schema "SCHEMA_API")




add_library             (Engine SHARED ${engine_src} ${engine_inc})
target_link_libraries   (Engine logger pvt.lzwdgc.polygon4.data_manager.data_manager dbghelp
    pvt.cppan.demo.boost.filesystem
    pvt.cppan.demo.lua
)
add_dependencies        (Engine clean_hotpatch ${api_name}_lua)

#set_target_properties(Engine PROPERTIES LINK_FLAGS "/WHOLEARCHIVE")

if (WIN32)
    #set(x)
    set(x "__declspec(dllexport)")
    target_compile_definitions(Engine
        PRIVATE P4_ENGINE_API=${x}
        #PRIVATE SCHEMA_API=${x}
        #PRIVATE DATA_MANAGER_API=${x}
    )
endif()


#set_target_properties   (Engine PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS True)

add_custom_command(TARGET Engine POST_BUILD
    COMMAND pdbfix $<TARGET_FILE:Engine>
    DEPENDS pdbfix Engine
)

if (WIN32)
    add_custom_command(TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Engine> ${CMAKE_SOURCE_DIR}/../../Binaries/Win64/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:Engine> ${CMAKE_SOURCE_DIR}/../../Binaries/Win64/

        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:pvt.lzwdgc.polygon4.data_manager.data_manager> ${CMAKE_SOURCE_DIR}/../../Binaries/Win64/
        #COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:pvt.lzwdgc.polygon4.data_manager.data_manager> ${CMAKE_SOURCE_DIR}/../../Binaries/Win64/

        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:pvt.lzwdgc.polygon4.data_manager.schema> ${CMAKE_SOURCE_DIR}/../../Binaries/Win64/
        #COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:pvt.lzwdgc.polygon4.data_manager.schema> ${CMAKE_SOURCE_DIR}/../../Binaries/Win64/
    )
endif()

# generate fiels for UE4
file(GENERATE
    OUTPUT ${PROJECT_BINARY_DIR}/includes.txt
    CONTENT $<TARGET_PROPERTY:Engine,INTERFACE_INCLUDE_DIRECTORIES>
)
file(GENERATE
    OUTPUT ${PROJECT_BINARY_DIR}/data_manager_$<CONFIG>.txt
    CONTENT $<TARGET_LINKER_FILE:pvt.lzwdgc.polygon4.data_manager.data_manager>
)
file(GENERATE
    OUTPUT ${PROJECT_BINARY_DIR}/schema_$<CONFIG>.txt
    CONTENT $<TARGET_LINKER_FILE:pvt.lzwdgc.polygon4.data_manager.schema>
)


########################################
# EXECUTABLE Fix PDB
########################################

add_executable          (pdbfix Tools/PdbFix.cpp)
target_link_libraries   (pdbfix dbghelp pvt.cppan.demo.boost.filesystem)
project_group           (pdbfix tools)


########################################
# EXECUTABLE Fix project
########################################

add_executable          (fixproject Tools/FixProject.cpp)
project_group           (fixproject tools)


########################################
# CUSTOM BUILD STEP clean_hotpatch
########################################

add_custom_target(clean_hotpatch
    COMMAND fixproject ${CMAKE_CURRENT_BINARY_DIR}/Engine.vcxproj.user Engine ${output_dir}/$(Configuration)/Engine
    DEPENDS fixproject
)
project_group           (clean_hotpatch tools)

################################################################################
