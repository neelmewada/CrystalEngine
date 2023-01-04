cmake_minimum_required(VERSION 3.20)

add_subdirectory(Source)

if(${PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED})
    #add_subdirectory(Tests)
endif()
