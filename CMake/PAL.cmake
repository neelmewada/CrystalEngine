
include_guard(GLOBAL)

if (CE_STANDALONE)
    
    if (${CE_STANDALONE} STREQUAL "Windows")

        set(PAL_STANDALONE_PLATFORM_NAME "Windows")
        set(CMAKE_SYSTEM_NAME "Windows")
        set(CE_STANDALONE ON)
        set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "Mac")

        set(PAL_STANDALONE_PLATFORM_NAME "Mac")
        set(CMAKE_SYSTEM_NAME "Darwin")
        set(CE_STANDALONE ON)
        set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "Linux")

        set(PAL_STANDALONE_PLATFORM_NAME "Linux")
        set(CMAKE_SYSTEM_NAME "Linux")
        set(CE_STANDALONE ON)
        set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "Android")

        set(PAL_STANDALONE_PLATFORM_NAME "Android")
        set(CMAKE_SYSTEM_NAME "Android")
        set(CE_STANDALONE ON)
        set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "iOS")

        set(PAL_STANDALONE_PLATFORM_NAME "iOS")
        set(CMAKE_SYSTEM_NAME "iOS")
        set(CE_STANDALONE ON)
        set(PAL_PLATFORM_NAME ${PAL_STANDALONE_PLATFORM_NAME})

    elseif(${CE_STANDALONE} STREQUAL "OFF")

        set(CE_STANDALONE OFF)
        set(PAL_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
        
    else()
        message(FATAL_ERROR "Invalid CE_STANDALONE platform passed: ${CE_STANDALONE}")
    endif()

else()
    set(PAL_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
    set(PAL_HOST_PLATFORM_NAME ${CMAKE_SYSTEM_NAME})
endif()

if (${CE_STANDALONE})
    message(STATUS "Standalone Build: ${PAL_PLATFORM_NAME}")
else()
    message(STATUS "Editor Build: ${PAL_PLATFORM_NAME}")
endif()





