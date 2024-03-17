
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::sdl")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "sdl")
set(PACKAGE_VERISON "2.30.1")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(LIB_NAME "SDL2")

set(${PACKAGE_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${PACKAGE_NAME}_INCLUDE_DIR ${${PACKAGE_NAME}_BASE_DIR}/SDL/include)
set(${PACKAGE_NAME}_LIBS_DIR ${${PACKAGE_NAME}_BASE_DIR}/SDL)

if (${PAL_PLATFORM_NAME} STREQUAL "Mac")
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEBUG   ${${PACKAGE_NAME}_LIBS_DIR}/Debug/lib${LIB_NAME}-2.0d.dylib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEV     ${${PACKAGE_NAME}_LIBS_DIR}/Development/lib${LIB_NAME}-2.0.dylib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_RELEASE ${${PACKAGE_NAME}_LIBS_DIR}/Release/lib${LIB_NAME}-2.0.dylib)

    set_target_properties(${TARGET_WITH_NAMESPACE}
        PROPERTIES
            IMPORTED_LOCATION "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${LIB_NAME}.dylib"
    )

elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEBUG   ${${PACKAGE_NAME}_LIBS_DIR}/Debug/${LIB_NAME}maind.lib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEV     ${${PACKAGE_NAME}_LIBS_DIR}/Development/${LIB_NAME}main.lib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_RELEASE ${${PACKAGE_NAME}_LIBS_DIR}/Release/${LIB_NAME}main.lib)

    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEBUG   ${${PACKAGE_NAME}_LIBS_DIR}/Debug/${LIB_NAME}d.lib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEV     ${${PACKAGE_NAME}_LIBS_DIR}/Development/${LIB_NAME}.lib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_RELEASE ${${PACKAGE_NAME}_LIBS_DIR}/Release/${LIB_NAME}.lib)

else()
    error("${PACKAGE_NAME} build not found for platform: ${PAL_PLATFORM_NAME}")
endif()

add_library(${TARGET_WITH_NAMESPACE} STATIC IMPORTED GLOBAL)

set_target_properties(${TARGET_WITH_NAMESPACE}
    PROPERTIES
        IMPORTED_LOCATION_DEBUG       ${${PACKAGE_NAME}_STATIC_LIBRARY_DEBUG}
        IMPORTED_LOCATION_DEVELOPMENT ${${PACKAGE_NAME}_STATIC_LIBRARY_DEV}
        IMPORTED_LOCATION_PROFILE     ${${PACKAGE_NAME}_STATIC_LIBRARY_DEV}
        IMPORTED_LOCATION_RELEASE     ${${PACKAGE_NAME}_STATIC_LIBRARY_RELEASE}
)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${PACKAGE_NAME}_INCLUDE_DIR}
        "${${PACKAGE_NAME}_BASE_DIR}/SDL/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/"
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

set(${PACKAGE_NAME}_BIN_DIR "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>")
set(${PACKAGE_NAME}_BIN_DIR ${${PACKAGE_NAME}_BIN_DIR} PARENT_SCOPE)

if(${PAL_PLATFORM_IS_WINDOWS})
    set(${PACKAGE_NAME}_RUNTIME_DEPS "${LIB_NAME}$<$<CONFIG:Debug>:d>.dll")
elseif(${PAL_PLATFORM_IS_MAC})
    set(${PACKAGE_NAME}_RUNTIME_DEPS "lib${LIB_NAME}-2.0$<$<CONFIG:Debug>:d>.dylib")
    set(${PACKAGE_NAME}_RUNTIME_DEPS "lib${LIB_NAME}-2.0$<$<CONFIG:Debug>:d>.0.dylib")
endif()
set(${PACKAGE_NAME}_RUNTIME_DEPS ${${PACKAGE_NAME}_RUNTIME_DEPS} PARENT_SCOPE)



