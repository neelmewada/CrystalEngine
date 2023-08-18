cmake_minimum_required(VERSION 3.20)

project(CoreGUI CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} SHARED
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER
        PRIVATE
            Private/${PROJECT_NAME}Internal.h
            Public/${PROJECT_NAME}.h
    FILES_CMAKE
        PRIVATE
            ${PROJECT_NAME_LOWERCASE}_private_files.cmake
        PUBLIC
            ${PROJECT_NAME_LOWERCASE}_public_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Private/
            Private/imgui
        PUBLIC
            Public/
    COMPILE_DEFINITIONS
        PRIVATE
            ${PROJECT_NAME}_EXPORTS
            IMGUI_API=DLL_IMPORT
    BUILD_DEPENDENCIES
        PRIVATE
        PUBLIC
            CE::Core
            CE::CoreRHI
            $<${PAL_TRAIT_VULKAN_SUPPORTED}:CE::VulkanRHI>
    RUNTIME_DEPENDENCIES

)


if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    add_subdirectory(Tests)
endif()

