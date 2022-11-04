
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::fmt")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "fmt")
set(PACKAGE_VERISON "9.1.0")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

set(LIB_NAME "fmt")

set(${LIB_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${LIB_NAME}_INCLUDE_DIR ${${LIB_NAME}_BASE_DIR}/fmt/include)
set(${LIB_NAME}_LIBS_DIR ${${LIB_NAME}_BASE_DIR}/fmt)

if (${PAL_PLATFORM_NAME} STREQUAL "Linux" OR ${PAL_PLATFORM_NAME} STREQUAL "Mac")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG        ${${LIB_NAME}_LIBS_DIR}/Debug/libfmtd.a)
    set(${LIB_NAME}_STATIC_LIBRARY_DEVELOPMENT  ${${LIB_NAME}_LIBS_DIR}/Development/libfmt.a)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE      ${${LIB_NAME}_LIBS_DIR}/Release/libfmt.a)
elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG        ${${LIB_NAME}_LIBS_DIR}/Debug/fmtd.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_DEVELOPMENT  ${${LIB_NAME}_LIBS_DIR}/Development/fmt.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE      ${${LIB_NAME}_LIBS_DIR}/Release/fmt.lib)
else()
    error("fmt build not found for platform: ${PAL_PLATFORM_NAME}")
endif()

add_library(${TARGET_WITH_NAMESPACE} STATIC IMPORTED GLOBAL)

set_target_properties(${TARGET_WITH_NAMESPACE}
    PROPERTIES
        IMPORTED_LOCATION_DEBUG       ${${LIB_NAME}_STATIC_LIBRARY_DEBUG}
        IMPORTED_LOCATION_DEVELOPMENT ${${LIB_NAME}_STATIC_LIBRARY_DEVELOPMENT}
        IMPORTED_LOCATION_PROFILE     ${${LIB_NAME}_STATIC_LIBRARY_DEVELOPMENT}
        IMPORTED_LOCATION_RELEASE     ${${LIB_NAME}_STATIC_LIBRARY_RELEASE}
)

set_target_properties(${TARGET_WITH_NAMESPACE}
    PROPERTIES
        IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/libfmtd.a"
)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

set(${LIB_NAME}_FOUND True)


