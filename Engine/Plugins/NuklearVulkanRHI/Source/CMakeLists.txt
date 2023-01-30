cmake_minimum_required(VERSION 3.20)

project(NuklearVulkanRHI)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_target(${PROJECT_NAME} SHARED
    NAMESPACE CE
    FOLDER Engine/Plugins
    PCHHEADER
        PUBLIC 
            Public/${PROJECT_NAME}.h
    OUTPUT_DIRECTORY
        "Engine/Plugins/NuklearVulkanRHI"
    COPY_CONFIGS
    FILES_CMAKE
        PRIVATE
            ${PROJECT_NAME_LOWERCASE}_private_files.cmake
        PUBLIC
            ${PROJECT_NAME_LOWERCASE}_public_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            ${Vulkan_INCLUDE_DIR}
            Private/
        PUBLIC
            Public/
    COMPILE_DEFINITIONS
        PRIVATE
            ${PROJECT_NAME}_EXPORTS
    BUILD_DEPENDENCIES
        PRIVATE
            CE::Core
            CE::System
            ${Vulkan_LIBRARIES}
            $<$<BOOL:${PAL_TRAIT_BUILD_EDITOR}>:Qt6::Core>
            $<$<BOOL:${PAL_TRAIT_BUILD_EDITOR}>:Qt6::Gui>
            $<$<BOOL:${PAL_TRAIT_BUILD_EDITOR}>:Qt6::Widgets>
        PUBLIC
            ThirdParty::sdl
    RUNTIME_DEPENDENCIES
        
)

add_dependencies(EnginePlugins ${PROJECT_NAME})
