
include_guard(GLOBAL)

function(ce_add_configs NAME)

    set(options "")
    set(oneValueArgs FOLDER OUTPUT_DIRECTORY)
    set(multiValueArgs CONFIGS)

    cmake_parse_arguments(ce_add_configs "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(DEST_CFG_FILES "")

    foreach(cfg_file ${ce_add_configs_CONFIGS})
        cmake_path(RELATIVE_PATH cfg_file OUTPUT_VARIABLE dest_cfg_file_rel)
        set(dest_cfg_file "${CE_OUTPUT_DIR}/${ce_add_configs_OUTPUT_DIRECTORY}/${dest_cfg_file_rel}")
        list(APPEND DEST_CFG_FILES ${dest_cfg_file})

        cmake_path(GET dest_cfg_file PARENT_PATH dest_cfg_file_parent)

        add_custom_command(OUTPUT ${dest_cfg_file}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${cfg_file}" "${dest_cfg_file_parent}"
            DEPENDS ${cfg_file}
            VERBATIM
        )
        
    endforeach()

    add_custom_target(${NAME}
        DEPENDS ${DEST_CFG_FILES}
        SOURCES ${ce_add_configs_CONFIGS}
        VERBATIM
    )

    # add_custom_target(${NAME}
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${CE_OUTPUT_DIR}/${ce_add_configs_FOLDER}
    #     COMMAND ${CMAKE_COMMAND} -E rm -rf ${CE_OUTPUT_DIR}/${ce_add_configs_FOLDER}/CMakeLists.txt
    #     SOURCES ${ce_add_configs_CONFIGS}
    # )

    ce_group_sources_by_folder(${NAME})

    set_target_properties(${NAME} 
        PROPERTIES
            FOLDER "${ce_add_configs_FOLDER}"
    )

    
endfunction()

