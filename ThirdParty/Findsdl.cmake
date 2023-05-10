
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::sdl")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "sdl")
set(PACKAGE_VERISON "2.26.1")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(LIB_NAME "SDL2")

set(${LIB_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${LIB_NAME}_INCLUDE_DIR ${${LIB_NAME}_BASE_DIR}/SDL/include)
set(${LIB_NAME}_LIBS_DIR ${${LIB_NAME}_BASE_DIR}/SDL)

if (${PAL_PLATFORM_NAME} STREQUAL "Linux" OR ${PAL_PLATFORM_NAME} STREQUAL "Mac")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/lib${LIB_NAME}d.a)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/lib${LIB_NAME}.a)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/lib${LIB_NAME}.a)
elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/${LIB_NAME}-staticd.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/${LIB_NAME}-static.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/${LIB_NAME}-static.lib)
else()
    error("${PACKAGE_NAME} build not found for platform: ${PAL_PLATFORM_NAME}")
endif()

add_library(${TARGET_WITH_NAMESPACE} STATIC IMPORTED GLOBAL)

set_target_properties(${TARGET_WITH_NAMESPACE}
    PROPERTIES
        IMPORTED_LOCATION_DEBUG       ${${LIB_NAME}_STATIC_LIBRARY_DEBUG}
        IMPORTED_LOCATION_DEVELOPMENT ${${LIB_NAME}_STATIC_LIBRARY_DEV}
        IMPORTED_LOCATION_PROFILE     ${${LIB_NAME}_STATIC_LIBRARY_DEV}
        IMPORTED_LOCATION_RELEASE     ${${LIB_NAME}_STATIC_LIBRARY_RELEASE}
)

set_target_properties(${TARGET_WITH_NAMESPACE}
    PROPERTIES
        IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${LIB_NAME}.a"
)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

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


set(${LIB_NAME}_FOUND True)


