include_guard(GLOBAL)

function(ce_add_shaders NAME)
    set(options AUTORTTI COPY_CONFIGS VS_STARTUP_PROJECT)
    set(oneValueArgs VERSION OUTPUT_SUBDIRECTORY FOLDER NAMESPACE OUTPUT_DIRECTORY)
    set(multiValueArgs PCHHEADER FILES_CMAKE COMPILE_DEFINITIONS INCLUDE_DIRECTORIES BUILD_DEPENDENCIES RUNTIME_DEPENDENCIES)

    cmake_parse_arguments(ce_add_shaders ${options} ${oneValueArgs} ${multiValueArgs} ${ARGN})

    
    
endfunction()

