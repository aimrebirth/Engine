###############################################################################
#
# macros and functions
#
###############################################################################

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

#######################################
# FUNCTION project_group
#######################################
function(project_group target name)
    set_target_properties(${target} PROPERTIES FOLDER ${name})
endfunction(project_group)

################################################################################