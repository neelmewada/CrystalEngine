cmake_minimum_required(VERSION 3.20)

project(VulkanRHI CXX)

ce_add_target(${PROJECT_NAME} SHARED
    NAMESPACE CE
    FOLDER Engine/Source
    PCHHEADER 
        PUBLIC 
            Public/${PROJECT_NAME}.h
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
            ${Vulkan_LIBRARIES}
            $<$<BOOL:${PAL_TRAIT_BUILD_EDITOR}>:Qt6::Core>
            $<$<BOOL:${PAL_TRAIT_BUILD_EDITOR}>:Qt6::Gui>
            $<$<BOOL:${PAL_TRAIT_BUILD_EDITOR}>:Qt6::Widgets>
        PUBLIC
            CE::Core
            CE::System
        TARGETS Config
        MACFRAMEWORKS
            QuartzCore
    RUNTIME_DEPENDENCIES
        
)


if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    #add_subdirectory(Tests)
endif()

