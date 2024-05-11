cmake_minimum_required(VERSION 3.20)

project(Core CXX)

ce_add_target(Core SHARED
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER 
        PRIVATE 
            Public/Core.h
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
            ThirdParty::mINI
            ThirdParty::zip
            ThirdParty::crcpp
        PUBLIC
            ThirdParty::spdlog
            ThirdParty::yaml
            ThirdParty::efsw
        TARGETS Config
    RUNTIME_DEPENDENCIES
        
)

add_subdirectory(Tests)

