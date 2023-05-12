# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::dxcompiler")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "dxcompiler")
set(PACKAGE_VERISON "1.7.2212")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(LIB_NAME "dxcompiler")

set(${LIB_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${LIB_NAME}_INCLUDE_DIR ${${LIB_NAME}_BASE_DIR}/${PACKAGE_NAME}/Include)
set(${LIB_NAME}_LIBS_DIR ${${LIB_NAME}_BASE_DIR}/${PACKAGE_NAME})

if (${PAL_PLATFORM_NAME} STREQUAL "Mac")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/lib${LIB_NAME}.dylib)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/lib${LIB_NAME}.dylib)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/lib${LIB_NAME}.dylib)
elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/${LIB_NAME}.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/${LIB_NAME}.lib)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/${LIB_NAME}.lib)
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
        IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${LIB_NAME}.dylib"
)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

# Runtime Dependencies

ce_add_rt_deps(dxcompiler
    ROOT_PATH "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>"
    COPY_LIBS
        dxcompiler
    COPY_FILES
        $<${PAL_PLATFORM_IS_WINDOWS}:dxcompiler.pdb>
        $<${PAL_PLATFORM_IS_WINDOWS}:dxcompiler.exp>
        $<${PAL_PLATFORM_IS_WINDOWS}:dxc.exe>
        $<${PAL_PLATFORM_IS_WINDOWS}:dxc.pdb>
)


set(${LIB_NAME}_FOUND True)
