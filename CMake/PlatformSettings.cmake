
include_guard(GLOBAL)

if (DEFINED CE_STANDALONE)
    
    if (${CE_STANDALONE} STREQUAL "Windows")

        set(CE_STANDALONE_PLATFORM "Windows")
        set(CMAKE_SYSTEM_NAME "Windows")
        set(CE_STANDALONE ON)
        set(CE_PLATFORM ${CE_STANDALONE_PLATFORM})

    elseif(${CE_STANDALONE} STREQUAL "Mac")

        set(CE_STANDALONE_PLATFORM "Mac")
        set(CMAKE_SYSTEM_NAME "Darwin")
        set(CE_STANDALONE ON)
        set(CE_PLATFORM ${CE_STANDALONE_PLATFORM})

    elseif(${CE_STANDALONE} STREQUAL "Linux")

        set(CE_STANDALONE_PLATFORM "Linux")
        set(CMAKE_SYSTEM_NAME "Linux")
        set(CE_STANDALONE ON)
        set(CE_PLATFORM ${CE_STANDALONE_PLATFORM})

    elseif(${CE_STANDALONE} STREQUAL "Android")

        set(CE_STANDALONE_PLATFORM "Android")
        set(CMAKE_SYSTEM_NAME "Android")
        set(CE_STANDALONE ON)
        set(CE_PLATFORM ${CE_STANDALONE_PLATFORM})

    elseif(${CE_STANDALONE} STREQUAL "iOS")

        set(CE_STANDALONE_PLATFORM "iOS")
        set(CMAKE_SYSTEM_NAME "iOS")
        set(CE_STANDALONE ON)
        set(CE_PLATFORM ${CE_STANDALONE_PLATFORM})

    elseif(${CE_STANDALONE} STREQUAL "OFF")

        set(CE_STANDALONE OFF)
        set(CE_PLATFORM ${CMAKE_SYSTEM_NAME})
        
    else()
        message(FATAL_ERROR "Invalid CE_STANDALONE platform passed: ${CE_STANDALONE}")
    endif()

else()
    set(CE_PLATFORM ${CMAKE_SYSTEM_NAME})
endif()

if (${CE_STANDALONE})
    message(STATUS "Standalone Build: ${CE_PLATFORM}")
else()
    message(STATUS "Editor Build: ${CE_PLATFORM}")
endif()




