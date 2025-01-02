cmake_minimum_required(VERSION 3.20)

project(ThirdParty)

include(FindxxHash.cmake)
include(Findspdlog.cmake)
include(Findcxxopts.cmake)
include(Findefsw.cmake)
include(Findzip.cmake)
include(Findcrcpp.cmake)
include(Findstb.cmake)
include(Findminiz.cmake)
include(Findfreetype.cmake)
include(Findassimp.cmake)
include(Findsdl.cmake)
include(Findcpptrace.cmake)
include(Findispctexturecompressor.cmake)
#include(Findclipper.cmake)
include(Findtracy.cmake)

# Linux packages
# libxxhash-dev 


if(${PAL_PLATFORM_IS_WINDOWS})
    find_library(Vulkan_dxc_DEBUG_LIBRARY
        NAMES dxcompilerd
        HINTS
        ${_Vulkan_hint_library_search_paths})

    mark_as_advanced(Vulkan_dxc_DEBUG_LIBRARY)

    cmake_path(GET Vulkan_dxc_LIBRARY PARENT_PATH Vulkan_dxc_DEBUG_LIBRARY)
    set(Vulkan_dxc_DEBUG_LIBRARY "${Vulkan_dxc_DEBUG_LIBRARY}/dxcompilerd.lib")

    set_property(TARGET Vulkan::dxc_lib APPEND
        PROPERTY
        IMPORTED_CONFIGURATIONS Debug)
    set_property(TARGET Vulkan::dxc_lib
        PROPERTY
        IMPORTED_LOCATION_DEBUG "${Vulkan_dxc_DEBUG_LIBRARY}")

    message(STATUS "Vulkan DXC Debug: ${Vulkan_dxc_DEBUG_LIBRARY}")
endif()


