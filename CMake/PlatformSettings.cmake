
include_guard(GLOBAL)

set(CE_HOST_PLATFORM ${CMAKE_SYSTEM_NAME})

if("${CE_STANDALONE_PLATFORM}" STREQUAL "Mac")
    set(CE_STANDALONE_PLATFORM "Darwin" CACHE STRING "Target platform for standalone build")
elseif("${CE_STANDALONE_PLATFORM}" STREQUAL "Windows")
    set(CE_STANDALONE_PLATFORM "Windows" CACHE STRING "Target platform for standalone build")
elseif("${CE_STANDALONE_PLATFORM}" STREQUAL "Linux")
    set(CE_STANDALONE_PLATFORM "Linux" CACHE STRING "Target platform for standalone build")
elseif("${CE_STANDALONE_PLATFORM}" STREQUAL "Android")
    set(CE_STANDALONE_PLATFORM "Android" CACHE STRING "Target platform for standalone build")
elseif("${CE_STANDALONE_PLATFORM}" STREQUAL "iOS")
    set(CE_STANDALONE_PLATFORM "iOS" CACHE STRING "Target platform for standalone build")
else()
    set(CE_STANDALONE_PLATFORM "${CMAKE_SYSTEM_NAME}" CACHE STRING "Target platform for standalone build")
endif()

message(STATUS "Host: ${CE_HOST_PLATFORM}")
message(STATUS "Standalone: ${CE_STANDALONE_PLATFORM}")

if("${CE_HOST_PLATFORM}" STREQUAL "Windows")
    if("${CE_STANDALONE_PLATFORM}" STREQUAL "Mac")
        message(FATAL_ERROR "ERROR: Can't build for Mac on Windows host")
    elseif("${CE_STANDALONE_PLATFORM}" STREQUAL "Linux")
        message(FATAL_ERROR "ERROR: Can't build for Linux on Windows host")
    elseif("${CE_STANDALONE_PLATFORM}" STREQUAL "iOS")
        message(FATAL_ERROR "ERROR: Can't build for iOS on Windows host")
    endif()
endif()



