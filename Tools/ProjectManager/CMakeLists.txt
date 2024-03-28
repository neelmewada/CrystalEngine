cmake_minimum_required(VERSION 3.20)

return()

project(ProjectManager)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} CONSOLEAPP
    NAMESPACE "CE::Tools"
    FOLDER Tools
    OUTPUT_DIRECTORY "Tools"
    FILES_CMAKE
        PRIVATE ${PROJECT_NAME_LOWERCASE}_sources.cmake
    BUILD_DEPENDENCIES
        PRIVATE
            CE::Core
            CE::System
            ThirdParty::cxxopts
    RUNTIME_DEPENDENCIES

)

