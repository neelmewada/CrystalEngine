cmake_minimum_required(VERSION 3.20)

project(Engine)

# Engine Config
add_subdirectory(Config)

# Engine Modules
add_subdirectory(Source/Core)
add_subdirectory(Source/System)

add_subdirectory(Source/GameSystem)
add_subdirectory(Source/GameLauncher)

# Plugins
add_subdirectory(Plugins/NuklearRHI)
add_subdirectory(Plugins/NuklearVulkanRHI)
