
include_guard(GLOBAL)

function(ce_add_configs NAME)

    set(options "")
    set(oneValueArgs FOLDER)
    set(multiValueArgs CONFIGS)

    cmake_parse_arguments(ce_add_configs "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_custom_target(${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${CE_OUTPUT_DIR}/${ce_add_configs_FOLDER}
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CE_OUTPUT_DIR}/${ce_add_configs_FOLDER}/CMakeLists.txt
        SOURCES ${ce_add_configs_CONFIGS}
    )

    set_target_properties(${PROJECT_NAME} 
        PROPERTIES
            FOLDER "${ce_add_configs_FOLDER}"
    )

    
endfunction()

