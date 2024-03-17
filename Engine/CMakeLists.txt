cmake_minimum_required(VERSION 3.20)

project(Engine)
set(CURRENT_PACKAGE_CATEGORY "Engine")

# Assets
add_subdirectory(Assets)

# Shaders
add_subdirectory(Shaders)

######################################
# Core Modules

add_subdirectory(Source/Core)
add_subdirectory(Source/CoreApplication)
add_subdirectory(Source/CoreInput)
add_subdirectory(Source/CoreMedia)
add_subdirectory(Source/CoreMesh)
add_subdirectory(Source/CoreRHI)
add_subdirectory(Source/CoreRPI)
#add_subdirectory(Source/CoreGUI)
#add_subdirectory(Source/CoreWidgets)
add_subdirectory(Source/CoreSettings)

# Core Dependencies
if(${PAL_TRAIT_VULKAN_SUPPORTED})
add_subdirectory(Source/VulkanRHI)
endif()

add_subdirectory(Source/CoreShader)

######################################
# System Modules

add_subdirectory(Source/System)
add_subdirectory(Source/CrystalWidgets)

if(${CE_STANDALONE})
    add_subdirectory(Source/VulkanSandbox)

    add_subdirectory(Source/GameSystem)
    if(${CE_BUILD_TESTS})
        add_subdirectory(Source/Sandbox)
    endif()
    add_subdirectory(Source/GameLauncher)
endif()
