cmake_minimum_required(VERSION 3.20)

project(AssetProcessor CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} TOOL
    AUTORTTI
    NAMESPACE "CE::Tools"
    FOLDER Tools
    PCHHEADER
        PRIVATE
            Source/${PROJECT_NAME}.h
    FILES_CMAKE
        PRIVATE
            ${PROJECT_NAME_LOWERCASE}_source_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Source/
    COMPILE_DEFINITIONS
        PRIVATE
            ${PROJECT_NAME}_EXPORTS
    BUILD_DEPENDENCIES
        PRIVATE
            CE::Core
            CE::System
            CE::EditorCore
    RUNTIME_DEPENDENCIES

)


if(${PAL_TRAIT_BUILD_TESTS})
    add_subdirectory(Tests)
endif()

