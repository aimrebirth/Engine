#
# Polygon-4
#

################################################################################

########################################
# FUNCTION set_output_directory
########################################
function(set_output_directory target dir)
    if (MSVC OR XCODE)
        foreach(conf ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${conf} conf2)
            set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${conf2} ${dir})
            set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${conf2} ${dir})
            set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${conf2} ${dir})
        endforeach()
    else() # single configuration
        add_custom_command(TARGET ${target} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${dir})
        set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${dir})
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${dir})
        set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${dir})
    endif()
endfunction(set_output_directory)

################################################################################


########################################
# LIBRARY common
########################################

add_library(common common/memory.cpp)

file(GLOB engine_inc "${CMAKE_SOURCE_DIR}/include/polygon4/*")
file(GLOB engine_src "engine/*")


########################################
# SHARED LIBRARY Engine
########################################

add_library(Engine SHARED ${engine_src} ${engine_inc})
target_link_libraries(Engine common ${Boost_LIBRARIES})
set_output_directory(Engine ${library_dir})

################################################################################