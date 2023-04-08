cmake_minimum_required(VERSION 3.20)

if(NOT ${PAL_TRAIT_METAL_SUPPORTED})
    return()
endif()

add_subdirectory(Source)

if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    #add_subdirectory(Tests)
endif()
