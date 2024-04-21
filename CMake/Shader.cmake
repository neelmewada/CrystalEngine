include_guard(GLOBAL)

function(ce_add_shaders NAME)
    set(options AUTORTTI COPY_CONFIGS VS_STARTUP_PROJECT)
    set(oneValueArgs VERSION OUTPUT_SUBDIRECTORY FOLDER)
    set(multiValueArgs SOURCES COMPILE_DEFINITIONS INCLUDE_DIRECTORIES)

    cmake_parse_arguments(ce_add_shaders "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_custom_target(${NAME}
        VERBATIM
        SOURCES ${ce_add_shaders_SOURCES}
    )

    ce_group_sources_by_folder(${NAME})

    if(ce_add_shaders_FOLDER)
        set_target_properties(${NAME} 
            PROPERTIES
                FOLDER "${ce_add_shaders_FOLDER}"
        )
    endif()
    
endfunction()

