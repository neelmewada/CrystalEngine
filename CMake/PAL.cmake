
include_guard(GLOBAL)

# Set defaults before loading any PAL_*.cmake
ce_set(PAL_PLATFORM_IS_MAC 0)
ce_set(PAL_PLATFORM_IS_WINDOWS 0)

# Get platform name maps
file(GLOB detection_files "CMake/Platform/*/PALDetection_*.cmake")
foreach(detection_file ${detection_files})
    include(${detection_file})
endforeach()

set(CE_EDITOR_PATH "${CMAKE_BINARY_DIR}/$<CONFIG>" CACHE STRING "Path to editor build folder, where all the editor binaries exist")

if (CE_STANDALONE)
    
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Windows")
        ce_set(CMAKE_SYSTEM_NAME "Windows")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Mac")
        ce_set(CMAKE_SYSTEM_NAME "Darwin")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Linux")
        ce_set(CMAKE_SYSTEM_NAME "Linux")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Android")
        ce_set(CMAKE_SYSTEM_NAME "Android")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "iOS")
        ce_set(CMAKE_SYSTEM_NAME "iOS")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})
        
    else()

        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
        ce_set(PAL_HOST_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})

    endif()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(PAL_HOST_PLATFORM_NAME "Mac")
    else()
        set(PAL_HOST_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
    endif()

else()
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(PAL_PLATFORM_NAME "Mac")
        set(PAL_HOST_PLATFORM_NAME "Mac")
    else()
        set(PAL_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
        set(PAL_HOST_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
    endif()
endif()

if (${CE_STANDALONE}) # Standalone build (runtime)
    message(STATUS "Standalone Build: ${PAL_PLATFORM_NAME}")
    add_compile_definitions(CE_STANDALONE=1)
else() # Editor build (host)
    message(STATUS "Editor Build: ${PAL_PLATFORM_NAME}")
    add_compile_definitions(CE_EDITOR_BUILD=1)
endif()

string(TOLOWER ${PAL_PLATFORM_NAME} PAL_PLATFORM_NAME_LOWERCASE)
string(TOLOWER ${PAL_HOST_PLATFORM_NAME} PAL_HOST_PLATFORM_NAME_LOWERCASE)

string(TOLOWER ${PAL_PLATFORM_NAME} PAL_PLATFORM_NAME_LOWERCASE)
ce_set(PAL_PLATFORM_NAME_LOWERCASE ${PAL_PLATFORM_NAME_LOWERCASE})

string(TOLOWER ${PAL_HOST_PLATFORM_NAME} PAL_HOST_PLATFORM_NAME_LOWERCASE)
ce_set(PAL_HOST_PLATFORM_NAME_LOWERCASE ${PAL_HOST_PLATFORM_NAME_LOWERCASE})

# Include Platform CMake
include(CMake/Platform/${PAL_PLATFORM_NAME}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Options

set(CE_HOST_BUILD_DIR "Build/${PAL_HOST_PLATFORM_NAME}" CACHE STRING "Path to the editor/tools build directory. Required when building standalone.")
if(NOT ${CE_STANDALONE})
    set(CE_HOST_BUILD_DIR "${CMAKE_BINARY_DIR}")
endif()

set(CE_HOST_TOOLS_BINARY_DIR "${CE_HOST_BUILD_DIR}")

if(IS_ABSOLUTE ${CE_HOST_BUILD_DIR})
    # Do nothing
else()
    # Get absolute path to host build directory
    set(CE_HOST_TOOLS_BINARY_DIR "${CE_ROOT_DIR}/${CE_HOST_TOOLS_BINARY_DIR}")
endif()

if(${CE_STANDALONE})
    message(STATUS "Host Tools Path: ${CE_HOST_TOOLS_BINARY_DIR}")
endif()


if(${CE_BUILD_TESTS})
    set(CE_BUILD_TESTS 1)
else()
    set(CE_BUILD_TESTS 0)
endif()


# Endianness

add_compile_definitions(PAL_TRAIT_${CMAKE_CXX_BYTE_ORDER}=1)

# Utils

function(ce_filter_platform_files FILES_LIST)
    if(NOT ${PAL_PLATFORM_NAME} STREQUAL "Windows")
        list(FILTER ${FILES_LIST} EXCLUDE REGEX ".*/PAL/Windows/.*")
    endif()
    
    if(NOT ${PAL_PLATFORM_NAME} STREQUAL "Mac")
        list(FILTER ${FILES_LIST} EXCLUDE REGEX ".*/PAL/Mac/.*")
    endif()

    if(NOT ${PAL_PLATFORM_NAME} STREQUAL "Linux")
        list(FILTER ${FILES_LIST} EXCLUDE REGEX ".*/PAL/Linux/.*")
    endif()

    if(NOT ${PAL_PLATFORM_NAME} STREQUAL "Android")
        list(FILTER ${FILES_LIST} EXCLUDE REGEX ".*/PAL/Android/.*")
    endif()

    if(NOT ${PAL_PLATFORM_NAME} STREQUAL "iOS")
        list(FILTER ${FILES_LIST} EXCLUDE REGEX ".*/PAL/iOS/.*")
    endif()

    if(NOT ${PAL_TRAIT_QT_SUPPORTED})
        list(FILTER ${FILES_LIST} EXCLUDE REGEX ".*/PAL/Qt/.*")
    endif()
    
    if(NOT ${PAL_TRAIT_SDL_SUPPORTED})
        list(FILTER ${FILES_LIST} EXCLUDE REGEX ".*/PAL/SDL/.*")
    endif()
    
    set(${FILES_LIST} ${${FILES_LIST}} PARENT_SCOPE)
endfunction()

# DEPRECATED: Use ce_filter_platform_files instead!
function(ce_exclude_platform_files FILES_LIST)
    ce_filter_platform_files("${FILES_LIST}")
    set(${FILES_LIST} ${${FILES_LIST}} PARENT_SCOPE)
endfunction()


