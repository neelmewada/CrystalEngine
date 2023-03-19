
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::spdlog")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "spdlog")
set(PACKAGE_VERISON "1.10.0")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(LIB_NAME "spdlog")

set(${LIB_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${LIB_NAME}_INCLUDE_DIR ${${LIB_NAME}_BASE_DIR}/spdlog/include)
set(${LIB_NAME}_LIBS_DIR ${${LIB_NAME}_BASE_DIR}/spdlog)

if (${PAL_PLATFORM_NAME} STREQUAL "Linux" OR ${PAL_PLATFORM_NAME} STREQUAL "Mac")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/libspdlogd.a)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/libspdlog.a)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/libspdlog.a)
elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/spdlogd.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/spdlog.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/spdlog.lib)
else()
    error("spdlog build not found for platform: ${PAL_PLATFORM_NAME}")
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
        IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/libfmtd.a"
)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

set(${LIB_NAME}_FOUND True)

