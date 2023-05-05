
cmake_minimum_required(VERSION 3.20)

project(Sandbox CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} CONSOLEAPP
    NAMESPACE CE
    FOLDER Engine/Source
    FILES_CMAKE
        PRIVATE ${PROJECT_NAME_LOWERCASE}_source_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Source/
    BUILD_DEPENDENCIES
        PRIVATE
            CE::Core
    RUNTIME_DEPENDENCIES

)



