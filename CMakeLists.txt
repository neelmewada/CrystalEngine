cmake_minimum_required(VERSION 3.20)

set (CMAKE_CXX_STANDARD 20)

if(NOT PROJECT_NAME)
    project(CrystalEngine
        LANGUAGES C CXX)
endif()

include(CMake/CESet.cmake)
include(CMake/Macros.cmake)
include(CMake/Version.cmake)

ce_set(CE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

################################################################################
# Initialize
################################################################################
include(CMake/GeneralSettings.cmake)
include(CMake/PAL.cmake)
include(CMake/Configurations.cmake)
include(CMake/OutputDirectory.cmake)
include(CMake/Targets.cmake)
include(CMake/ThirdParty.cmake)

# Import 3rd Party libraries
add_subdirectory(ThirdParty)

# Build Engine
add_subdirectory(Engine)

# Build Host Libraries
if(${PAL_TRAIT_BUILD_HOST_LIBRARIES})
    add_subdirectory(Libraries)
endif()

# Build Editor
if(${PAL_TRAIT_BUILD_EDITOR})
    add_subdirectory(Editor)
endif()

# Build Host Tools
if(${PAL_TRAIT_BUILD_HOST_TOOLS})
    add_subdirectory(Tools)
endif()

