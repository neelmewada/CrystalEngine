cmake_minimum_required(VERSION 3.20)

project(NuklearRHI)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} SHARED
    NAMESPACE CE
    FOLDER Engine/Plugins
    PCHHEADER
        PUBLIC 
            Public/${PROJECT_NAME}.h
    OUTPUT_DIRECTORY
        "Engine/Plugins/NuklearRHI"
    COPY_CONFIGS
    FILES_CMAKE
        PRIVATE
            ${PROJECT_NAME_LOWERCASE}_private_files.cmake
        PUBLIC
            ${PROJECT_NAME_LOWERCASE}_public_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Private/
        PUBLIC
            Public/
    COMPILE_DEFINITIONS
        PRIVATE
            ${PROJECT_NAME}_EXPORTS
    BUILD_DEPENDENCIES
        PRIVATE
            CE::Core
            CE::System
    RUNTIME_DEPENDENCIES
        
)

add_dependencies(EnginePlugins ${PROJECT_NAME})

