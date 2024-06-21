
include_guard(GLOBAL)

function(ce_add_resources NAME)

    set(options "")
    set(oneValueArgs FOLDER OUTPUT_DIRECTORY)
    set(multiValueArgs RESOURCES)

    cmake_parse_arguments(ce_add_resources "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(DEST_FILES "")

    foreach(file ${ce_add_resources_RESOURCES})
        cmake_path(RELATIVE_PATH file OUTPUT_VARIABLE dest_file_rel)
        set(dest_file "${CE_OUTPUT_DIR}/${ce_add_resources_OUTPUT_DIRECTORY}/${dest_file_rel}")
        list(APPEND DEST_FILES ${dest_file})

        cmake_path(GET dest_file PARENT_PATH dest_file_parent)

        add_custom_command(OUTPUT ${dest_file}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${file}" "${dest_file_parent}"
            DEPENDS ${file}
            VERBATIM
        )
        
    endforeach()

    add_custom_target(${NAME}
        DEPENDS ${DEST_FILES}
        SOURCES ${ce_add_resources_RESOURCES}
        VERBATIM
    )
    ce_group_sources_by_folder(${NAME})

    set_target_properties(${NAME} 
        PROPERTIES
            FOLDER "${ce_add_resources_FOLDER}"
    )
    
endfunction()

