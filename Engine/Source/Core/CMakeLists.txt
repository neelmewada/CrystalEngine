cmake_minimum_required(VERSION 3.20)

project(Core CXX)

ce_add_target(Core SHARED
    NAMESPACE CE
    FOLDER Engine
    #GENERATED
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
            Core_EXPORTS
    BUILD_DEPENDENCIES
        PRIVATE
            ThirdParty::xxHash
)


if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    add_subdirectory(Tests)
endif()

