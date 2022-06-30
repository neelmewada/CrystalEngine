cmake_minimum_required(VERSION 3.22)

function(compile_vert_shaders VERT_SHADER_SOURCES)
    foreach(SRC ${VERT_SHADER_SOURCES})
        string(REGEX REPLACE "[.]vert$" "_vert.spv" OUT ${SRC})
        message(Shader Output: " ${OUT}")
        execute_process(COMMAND ${VK_BIN}/glslc --target-spv=spv1.3
                -o ${OUT}
                ${SRC}
                RESULT_VARIABLE GLSLC_RESULT)
    endforeach()
endfunction()

function(compile_frag_shaders FRAG_SHADER_SOURCES)
    foreach(SRC ${FRAG_SHADER_SOURCES})
        string(REGEX REPLACE "[.]frag$" "_frag.spv" OUT ${SRC})
        message(Shader Output: " ${OUT}")
        execute_process(COMMAND ${VK_BIN}/glslc --target-spv=spv1.3
                -o ${OUT}
                ${SRC}
                RESULT_VARIABLE GLSLC_RESULT)
    endforeach()
endfunction()

function(cmd_compile_shaders TARGET VERT_SHADER_SOURCES FRAG_SHADER_SOURCES)
    foreach(SRC ${VERT_SHADER_SOURCES})
        string(REGEX REPLACE "[.]vert$" "_vert.spv" OUT ${SRC})
        message(ShaderBuildOut: " ${OUT}")
        add_custom_command(TARGET ${TARGET} PRE_BUILD
                COMMAND ${VK_BIN}/glslc --target-spv=spv1.3 -o ${OUT} ${SRC}
                VERBATIM)
    endforeach()
    foreach(SRC ${FRAG_SHADER_SOURCES})
        string(REGEX REPLACE "[.]frag$" "_frag.spv" OUT ${SRC})
        message(ShaderBuildOut: " ${OUT}")
        add_custom_command(TARGET ${TARGET} PRE_BUILD
                COMMAND ${VK_BIN}/glslc --target-spv=spv1.3 -o ${OUT} ${SRC}
                VERBATIM)
    endforeach()
endfunction()

function(cmd_copy_shaders TARGET IN_DIR OUT_DIR)
    add_custom_command(TARGET ${TARGET} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${IN_DIR} ${OUT_DIR}
            COMMAND ${CMAKE_COMMAND} -E rm -r ${OUT_DIR}/*.vert ${OUT_DIR}/*.frag
            COMMENT "Copying to output directory")
endfunction()
