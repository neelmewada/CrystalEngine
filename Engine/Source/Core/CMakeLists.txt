cmake_minimum_required(VERSION 3.20)

project(Core CXX)

ce_add_target(Core SHARED
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER 
        PUBLIC 
            Public/Core.h
            Public/CoreMinimal.h
    FILES_CMAKE
        PRIVATE
            core_private_files.cmake
        PUBLIC
            core_public_files.cmake
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
            ThirdParty::xxHash
        PUBLIC
            ThirdParty::spdlog
            ThirdParty::yaml
    RUNTIME_DEPENDENCIES
        
)


if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    add_subdirectory(Tests)
endif()

