include_guard(GLOBAL)

function(ce_add_shaders NAME)
    set(options AUTORTTI COPY_CONFIGS VS_STARTUP_PROJECT)
    set(oneValueArgs VERSION OUTPUT_DIRECTORY FOLDER)
    set(multiValueArgs SOURCES COMPILE_DEFINITIONS INCLUDE_DIRECTORIES)

    set(DXC "$ENV{VULKAN_SDK}/bin/dxc")
    cmake_parse_arguments(ce_add_shaders "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(DEST_SHADERBIN_FILES "")

    foreach(hlsl_file ${ce_add_shaders_SOURCES})
        cmake_path(GET hlsl_file EXTENSION LAST_ONLY hlsl_file_ext)
        
        if(${hlsl_file_ext} STREQUAL ".hlsl")

            cmake_path(REMOVE_EXTENSION hlsl_file)
            cmake_path(GET hlsl_file FILENAME hlsl_file_name)
            cmake_path(RELATIVE_PATH hlsl_file OUTPUT_VARIABLE dest_shaderbin_file_rel)
            set(dest_shaderbin_file "${CE_OUTPUT_DIR}/${ce_add_shaders_OUTPUT_DIRECTORY}/${dest_shaderbin_file_rel}.shaderbin")
            list(APPEND DEST_SHADERBIN_FILES ${dest_shaderbin_file})

            cmake_path(GET dest_shaderbin_file PARENT_PATH dest_shaderbin_file_parent)

            add_custom_command(OUTPUT ${dest_shaderbin_file}
                COMMAND ${DXC} -spirv -E VertMain -T vs_6_0 -D PLATFORM_DESKTOP=1 -D COMPILE=1 -D VERTEX=1 -fspv-preserve-bindings -Fo "${dest_shaderbin_file_parent}/${hlsl_file_name}.vert.spv" "${hlsl_file}.hlsl"
                COMMAND ${DXC} -spirv -E FragMain -T ps_6_0 -D PLATFORM_DESKTOP=1 -D COMPILE=1 -D FRAGMENT=1 -fspv-preserve-bindings -Fo "${dest_shaderbin_file_parent}/${hlsl_file_name}.frag.spv" "${hlsl_file}.hlsl"
                COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${dest_shaderbin_file}" --format=zip "${dest_shaderbin_file_parent}/${hlsl_file_name}.vert.spv" "${dest_shaderbin_file_parent}/${hlsl_file_name}.frag.spv"
            )

            # add_custom_target(create_zip COMMAND
            #     ${CMAKE_COMMAND} -E tar "cfv" "archive.zip" --format=zip
            #     "${CMAKE_CURRENT_SOURCE_DIR}/testfile.txt"
            #     "${CMAKE_CURRENT_SOURCE_DIR}/testdir")

        endif()
    endforeach()

    add_custom_target(${NAME}
        DEPENDS ${DEST_SHADERBIN_FILES}
        SOURCES ${ce_add_shaders_SOURCES}
        VERBATIM
    )

    ce_group_sources_by_folder(${NAME})

    if(ce_add_shaders_FOLDER)
        set_target_properties(${NAME} 
            PROPERTIES
                FOLDER "${ce_add_shaders_FOLDER}"
        )
    endif()
    
endfunction()

