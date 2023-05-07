cmake_minimum_required(VERSION 3.20)

project(Engine)

# Assets
add_subdirectory(Assets)

######################################
# Core Modules

add_subdirectory(Source/Core)
add_subdirectory(Source/AutoRTTI)

add_subdirectory(Source/CoreMedia)

# RHI Modules
add_subdirectory(Source/CoreRHI)
if(${PAL_TRAIT_VULKAN_SUPPORTED})
    add_subdirectory(Source/VulkanRHI)
endif()

######################################
# System Modules

add_subdirectory(Source/System)

add_subdirectory(Source/GameSystem)
add_subdirectory(Source/GameLauncher)

# Sandbox
if(${CE_BUILD_TESTS})
    add_subdirectory(Source/Sandbox)
endif()
