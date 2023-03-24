cmake_minimum_required(VERSION 3.20)

project(ThirdParty)

# Utils
find_package(xxHash REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml REQUIRED)
find_package(mINI REQUIRED)
find_package(cxxopts REQUIRED)
find_package(efsw REQUIRED)
find_package(zip REQUIRED)

# SDL2
find_package(sdl REQUIRED)

# Qt
if(${PAL_TRAIT_BUILD_HOST_TOOLS})
    include(FindQt.cmake)
    find_package(qads REQUIRED)
    find_package(qtnodes REQUIRED) 
endif()


# Vulkan
if(${PAL_TRAIT_VULKAN_SUPPORTED})
    find_package(Vulkan REQUIRED)
    set(Vulkan_LIBRARIES ${Vulkan_LIBRARIES} PARENT_SCOPE)
endif()

# Metal
if(${PAL_TRAIT_METAL_SUPPORTED})
    find_package(metalcpp)
endif()

