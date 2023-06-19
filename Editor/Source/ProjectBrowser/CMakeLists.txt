cmake_minimum_required(VERSION 3.20)

project(ProjectBrowser CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} CONSOLEAPP
    AUTORTTI
    NAMESPACE CE
    FOLDER Editor/Source
    PCHHEADER
        PRIVATE
            Sources/Main.h
    FILES_CMAKE
        PRIVATE
            ${PROJECT_NAME_LOWERCASE}_source_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Sources/
    COMPILE_DEFINITIONS
        PRIVATE
            ${PROJECT_NAME}_EXPORTS
    BUILD_DEPENDENCIES
        PRIVATE
            ThirdParty::cxxopts
        PUBLIC
            CE::Core
            CE::CoreGUI
            CE::CoreWidgets
            CE::EditorCore
            CE::EditorWidgets
    RUNTIME_DEPENDENCIES

)


if(${PAL_TRAIT_BUILD_TESTS})
    add_subdirectory(Tests)
endif()

