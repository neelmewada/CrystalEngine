cmake_minimum_required(VERSION 3.20)

project(Engine)

# Assets
add_subdirectory(Assets)

# Engine Config
#add_subdirectory(Config)

# Engine Modules
add_subdirectory(Source/Core)
if(${PAL_TRAIT_BUILD_HOST_TOOLS})
    add_subdirectory(Source/AutoRTTI)
endif()
add_subdirectory(Source/CoreMedia)

add_subdirectory(Source/System)

if(${PAL_TRAIT_VULKAN_SUPPORTED})
    add_subdirectory(Source/VulkanRHI)
endif()

add_subdirectory(Source/GameSystem)
add_subdirectory(Source/GameLauncher)

# Engine Plugins
add_library(EnginePlugins INTERFACE)
add_subdirectory(Plugins)
