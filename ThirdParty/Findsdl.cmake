
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::sdl")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "sdl")
set(PACKAGE_VERISON "2.30.1")

if(${PAL_PLATFORM_IS_MAC})
    set(PACKAGE_VERISON "2.26.5")
endif()

set(LIB_NAME "SDL2")

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

add_subdirectory(vendor/${PACKAGE_NAME})

add_library(${TARGET_WITH_NAMESPACE} ALIAS SDL2main)

set_target_properties(SDL2main PROPERTIES FOLDER "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)



if(${PAL_PLATFORM_IS_MAC})
    target_link_libraries(${TARGET_WITH_NAMESPACE}
        INTERFACE
            "-framework Cocoa"
            "-framework CoreAudio"
            "-framework Carbon"
            "-framework AudioToolbox"
            "-framework AVFoundation"
            "-framework Metal"
            "-framework MetalKit"
            "-framework IOKit"
            "-framework CoreGraphics"
            "-framework CoreVideo"
            "-framework CoreHaptics"
            "-framework GameController"
            "-framework ForceFeedback"
            "iconv"
    )
endif()



