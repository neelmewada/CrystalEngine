
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::zlib")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "zlib")
set(PACKAGE_VERISON "1.2.13")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(LIB_NAME "zlib")

set(${LIB_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${LIB_NAME}_INCLUDE_DIR ${${LIB_NAME}_BASE_DIR}/zlib)
set(${LIB_NAME}_LIBS_DIR ${${LIB_NAME}_BASE_DIR}/zlib)

if (${PAL_PLATFORM_NAME} STREQUAL "Mac" OR ${PAL_PLATFORM_NAME} STREQUAL "Linux")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/lib${LIB_NAME}.a)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/lib${LIB_NAME}.a)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/lib${LIB_NAME}.a)

elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/${LIB_NAME}staticd.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/${LIB_NAME}static.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/${LIB_NAME}static.lib)

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

if(${PAL_PLATFORM_NAME} STREQUAL "Mac" OR ${PAL_PLATFORM_NAME} STREQUAL "Linux")
    set_target_properties(${TARGET_WITH_NAMESPACE}
        PROPERTIES
            IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${LIB_NAME}.a"
    )
endif()


target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

set(${LIB_NAME}_FOUND True)
