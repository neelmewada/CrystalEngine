cmake_minimum_required(VERSION 3.20)

project(AutoRTTI CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} TOOL
    NAMESPACE "CE::Tools"
    FOLDER Tools
    FILES_CMAKE
        PRIVATE ${PROJECT_NAME_LOWERCASE}_source_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Source/
    BUILD_DEPENDENCIES
        PRIVATE
            CE::Core
            ThirdParty::cxxopts
    RUNTIME_DEPENDENCIES

)

add_dependencies(EditorTools "CE::Tools::${PROJECT_NAME}")


