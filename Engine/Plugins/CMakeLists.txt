cmake_minimum_required(VERSION 3.20)

add_subdirectory(NuklearRHI)

if(${PAL_TRAIT_VULKAN_SUPPORTED})
    add_subdirectory(NuklearVulkanRHI)
endif()

if(${PAL_TRAIT_METAL_SUPPORTED})
    add_subdirectory(NuklearMetalRHI)
endif()


