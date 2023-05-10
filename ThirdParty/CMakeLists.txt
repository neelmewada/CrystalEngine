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
find_package(crcpp REQUIRED)
find_package(stb REQUIRED)

# SDL2
find_package(sdl REQUIRED)

# ImGui
find_package(imgui REQUIRED)

# Qt
if(${PAL_TRAIT_QT_SUPPORTED})
    include(FindQt.cmake)
    find_package(qads REQUIRED)
    find_package(qtnodes REQUIRED)
endif()


# Vulkan
if(${PAL_TRAIT_VULKAN_SUPPORTED})
    find_package(Vulkan REQUIRED)
    set(Vulkan_LIBRARIES ${Vulkan_LIBRARIES} PARENT_SCOPE)

    if(${PAL_PLATFORM_IS_WINDOWS})
        set(Vulkan_BIN_DIR "$ENV{VULKAN_SDK}/Bin")
        set(Vulkan_BIN_DIR ${Vulkan_BIN_DIR} PARENT_SCOPE)
        set(Vulkan_RUNTIME_DEPS "VkLayer_khronos_validation.dll")
        set(Vulkan_RUNTIME_DEPS ${Vulkan_RUNTIME_DEPS} PARENT_SCOPE)
    endif()
    
endif()

# Metal
if(${PAL_TRAIT_METAL_SUPPORTED})
    find_package(metalcpp)
endif()

