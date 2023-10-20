cmake_minimum_required(VERSION 3.20)

project(GameLauncher CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} GUIAPP
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
            CE::Core
            CE::CoreSettings
            CE::CoreApplication
            CE::CoreMedia
            CE::CoreShader
            CE::CoreRHI
            $<${PAL_TRAIT_VULKAN_SUPPORTED}:CE::VulkanRHI>
            CE::System
            CE::GameSystem
            $<${PAL_TRAIT_BUID}:Sandbox>
    RUNTIME_DEPENDENCIES
)


if(${PAL_TRAIT_BUILD_TESTS})
    add_subdirectory(Tests)
endif()

