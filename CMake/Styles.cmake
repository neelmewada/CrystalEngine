
include_guard(GLOBAL)

function(ce_add_styles NAME)

    set(options "")
    set(oneValueArgs FOLDER OUTPUT_DIRECTORY)
    set(multiValueArgs STYLES)

    cmake_parse_arguments(ce_add_styles "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(DEST_CSS_FILES "")

    foreach(css_file ${ce_add_styles_STYLES})
        cmake_path(RELATIVE_PATH css_file OUTPUT_VARIABLE dest_css_file_rel)
        set(dest_css_file "${CE_OUTPUT_DIR}/${ce_add_styles_OUTPUT_DIRECTORY}/${dest_css_file_rel}")
        list(APPEND DEST_CSS_FILES ${dest_css_file})

        cmake_path(GET dest_css_file PARENT_PATH dest_css_file_parent)

        add_custom_command(OUTPUT ${dest_css_file}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${css_file}" "${dest_css_file_parent}"
            DEPENDS ${css_file}
            VERBATIM
        )
        
    endforeach()

    add_custom_target(${NAME}
        DEPENDS ${DEST_CSS_FILES}
        SOURCES ${ce_add_styles_STYLES}
        VERBATIM
    )
    ce_group_sources_by_folder(${NAME})

    set_target_properties(${NAME} 
        PROPERTIES
            FOLDER "${ce_add_styles_FOLDER}"
    )
    
endfunction()

