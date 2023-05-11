
cmake_minimum_required(VERSION 3.20)

project(Sandbox CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} CONSOLEAPP
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER
        PRIVATE
            Source/SandboxLoop.h
    FILES_CMAKE
        PRIVATE ${PROJECT_NAME_LOWERCASE}_source_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Source/
    BUILD_DEPENDENCIES
        PRIVATE
            # Core Modules
            CE::Core
            CE::CoreApplication
            CE::CoreMedia
            CE::CoreRHI
            CE::CoreGUI
            $<${PAL_TRAIT_VULKAN_SUPPORTED}:CE::VulkanRHI>
    RUNTIME_DEPENDENCIES

)



