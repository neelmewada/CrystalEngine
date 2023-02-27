cmake_minimum_required(VERSION 3.20)

project(ThirdParty)

# Utils
find_package(xxHash REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml REQUIRED)
find_package(mINI REQUIRED)
find_package(cxxopts REQUIRED)
find_package(efsw REQUIRED)

# SDL2
find_package(sdl REQUIRED)

include(FindQt.cmake)
find_package(qads REQUIRED)
find_package(qtnodes REQUIRED)

# Vulkan
if(${PAL_TRAIT_VULKAN_SUPPORTED})
    find_package(Vulkan)
endif()

# Metal
if(${PAL_TRAIT_METAL_SUPPORTED})
    find_package(metalcpp)
endif()

