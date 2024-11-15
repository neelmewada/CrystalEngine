
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::ispctexturecompressor")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "ispctexturecompressor")
set(PACKAGE_VERISON "1.1.0")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(LIB_NAME "ispc_texcomp")

set(${PACKAGE_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${PACKAGE_NAME}_INCLUDE_DIR ${${PACKAGE_NAME}_BASE_DIR}/${PACKAGE_NAME}/Include)
set(${PACKAGE_NAME}_LIBS_DIR ${${PACKAGE_NAME}_BASE_DIR}/${PACKAGE_NAME})

add_library(${TARGET_WITH_NAMESPACE} STATIC IMPORTED GLOBAL)

if (${PAL_PLATFORM_NAME} STREQUAL "Mac")
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEBUG   ${${PACKAGE_NAME}_LIBS_DIR}/Debug/lib${LIB_NAME}.dylib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEV     ${${PACKAGE_NAME}_LIBS_DIR}/Development/lib${LIB_NAME}.dylib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_RELEASE ${${PACKAGE_NAME}_LIBS_DIR}/Release/lib${LIB_NAME}.dylib)

    set_target_properties(${TARGET_WITH_NAMESPACE}
        PROPERTIES
            IMPORTED_LOCATION "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${PACKAGE_NAME}.a"
    )

    ce_add_rt_deps(ispctexturecompressor
        ROOT_PATH "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>"
        COPY_LIBS
            ${LIB_NAME}
    )

elseif (${PAL_PLATFORM_NAME} STREQUAL "Linux")

    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEBUG   ${${PACKAGE_NAME}_LIBS_DIR}/Debug/lib${LIB_NAME}.so)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEV     ${${PACKAGE_NAME}_LIBS_DIR}/Development/lib${LIB_NAME}.so)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_RELEASE ${${PACKAGE_NAME}_LIBS_DIR}/Release/lib${LIB_NAME}.so)

    set_target_properties(${TARGET_WITH_NAMESPACE}
        PROPERTIES
            IMPORTED_LOCATION "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${LIB_NAME}.so"
    )

    ce_add_rt_deps(ispctexturecompressor
        ROOT_PATH "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>"
        COPY_LIBS
            ${LIB_NAME}
    )

elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEBUG   ${${PACKAGE_NAME}_LIBS_DIR}/Debug/${LIB_NAME}.lib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_DEV     ${${PACKAGE_NAME}_LIBS_DIR}/Development/${LIB_NAME}.lib)
    set(${PACKAGE_NAME}_STATIC_LIBRARY_RELEASE ${${PACKAGE_NAME}_LIBS_DIR}/Release/${LIB_NAME}.lib)

    set_target_properties(${TARGET_WITH_NAMESPACE}
        PROPERTIES
            IMPORTED_LOCATION "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/${LIB_NAME}.lib"
    )

    ce_add_rt_deps(ispctexturecompressor
        ROOT_PATH "${${PACKAGE_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>"
    )

else()
    error("${PACKAGE_NAME} build not found for platform: ${PAL_PLATFORM_NAME}")
endif()

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
)

set(${PACKAGE_NAME}_FOUND True)