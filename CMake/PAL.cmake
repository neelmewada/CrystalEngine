
include_guard(GLOBAL)

# Get platform name maps
file(GLOB detection_files "CMake/Platform/*/PALDetection_*.cmake")
foreach(detection_file ${detection_files})
    include(${detection_file})
endforeach()

if (CE_STANDALONE)
    
    if (${CE_STANDALONE} STREQUAL "Windows")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Windows")
        ce_set(CMAKE_SYSTEM_NAME "Windows")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "Mac")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Mac")
        ce_set(CMAKE_SYSTEM_NAME "Darwin")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "Linux")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Linux")
        ce_set(CMAKE_SYSTEM_NAME "Linux")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "Android")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "Android")
        ce_set(CMAKE_SYSTEM_NAME "Android")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "iOS")

        ce_set(PAL_STANDALONE_PLATFORM_NAME "iOS")
        ce_set(CMAKE_SYSTEM_NAME "iOS")
        ce_set(CE_STANDALONE ON)
        ce_set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "OFF")

        ce_set(CE_STANDALONE OFF)
        ce_set(PAL_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
        
    else()
        message(FATAL_ERROR "Invalid CE_STANDALONE platform passed: ${CE_STANDALONE}")
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

include(CMake/Platform/${PAL_PLATFORM_NAME}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

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


