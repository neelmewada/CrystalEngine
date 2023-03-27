cmake_minimum_required(VERSION 3.20)

project(System CXX)

ce_add_target(System SHARED
    AUTORTTI
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER 
        PRIVATE 
            Public/System.h
    FILES_CMAKE
        PRIVATE
            system_private_files.cmake
        PUBLIC
            system_public_files.cmake
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
            ThirdParty::sdl
        PUBLIC
            CE::Core
        TARGETS EngineConfig EngineAssets
    RUNTIME_DEPENDENCIES
        
)


if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    add_subdirectory(Tests)
endif()

