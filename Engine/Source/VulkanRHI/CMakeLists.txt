cmake_minimum_required(VERSION 3.20)

project(VulkanRHI CXX)

ce_add_target(${PROJECT_NAME} SHARED
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER
        PRIVATE
            Public/VulkanRHI.h
    FILES_CMAKE
        PRIVATE
            vulkanrhi_private_files.cmake
        PUBLIC
            vulkanrhi_public_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Private/
            ${Vulkan_INCLUDE_DIR}
        PUBLIC
            Public/
    COMPILE_DEFINITIONS
        PRIVATE
            ${PROJECT_NAME}_EXPORTS
    BUILD_DEPENDENCIES
        PRIVATE
            ThirdParty::sdl
            ThirdParty::imgui
            ${Vulkan_LIBRARIES}
        PUBLIC
            CE::Core
            CE::CoreApplication
            CE::CoreRHI
            CE::CoreMedia
        TARGETS Config
        MACFRAMEWORKS
            QuartzCore
    RUNTIME_DEPENDENCIES
        Vulkan
)


if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    #add_subdirectory(Tests)
endif()

