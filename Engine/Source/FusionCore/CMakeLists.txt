cmake_minimum_required(VERSION 3.20)

project(FusionCore CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} SHARED
    AUTORTTI
    RESOURCES
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER
        PRIVATE
            Public/${PROJECT_NAME}.h
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
            ThirdParty::freetype
            ThirdParty::stb
            ThirdParty::miniz
        PUBLIC
            CE::Core
            CE::CoreMedia
            CE::CoreApplication
            CE::CoreRHI
            CE::CoreRPI
        TARGETS EngineResources
    RUNTIME_DEPENDENCIES

)

if(${CE_BUILD_TESTS})
    add_subdirectory(Tests)
endif()
