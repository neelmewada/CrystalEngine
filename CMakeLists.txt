cmake_minimum_required(VERSION 3.20)

include(CMake/CESet.cmake)

################################################################################
# Initialize
################################################################################
include(CMake/GeneralSettings.cmake)
include(CMake/PAL.cmake)
include(CMake/Configurations.cmake)
include(CMake/OutputDirectory.cmake)
include(CMake/Targets.cmake)
include(CMake/ThirdParty.cmake)

add_subdirectory(ThirdParty)

if(${PAL_TRAIT_BUILD_HOST_TOOLS})
    add_subdirectory(Tools)
endif()



if(${PAL_TRAIT_BUILD_EDITOR})
    
endif()



