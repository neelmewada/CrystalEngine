
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::compressonator")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "compressonator")
set(PACKAGE_VERISON "4.4.19")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(LIB_NAME "compressonator")

set(${LIB_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${LIB_NAME}_INCLUDE_DIR ${${LIB_NAME}_BASE_DIR}/${PACKAGE_NAME}/cmp_compressonatorlib)
set(${LIB_NAME}_LIBS_DIR ${${LIB_NAME}_BASE_DIR}/${PACKAGE_NAME})

if (${PAL_PLATFORM_NAME} STREQUAL "Mac" OR ${PAL_PLATFORM_NAME} STREQUAL "Linux")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/libCMP_Compressonator.a)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/libCMP_Compressonator.a)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/libCMP_Compressonator.a)

    set_target_properties(${TARGET_WITH_NAMESPACE}
        PROPERTIES
            IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${LIB_NAME}.a"
    )

elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    add_library(CMP_Compressonator STATIC IMPORTED)
    set_target_properties(CMP_Compressonator
        PROPERTIES
            IMPORTED_LOCATION_DEBUG       ${${LIB_NAME}_LIBS_DIR}/Debug/CMP_Compressonator.lib
            IMPORTED_LOCATION_DEVELOPMENT ${${LIB_NAME}_LIBS_DIR}/Development/CMP_Compressonator.lib
            IMPORTED_LOCATION_PROFILE     ${${LIB_NAME}_LIBS_DIR}/Development/CMP_Compressonator.lib
            IMPORTED_LOCATION_RELEASE     ${${LIB_NAME}_LIBS_DIR}/Release/CMP_Compressonator.lib
    )

else()
    error("${PACKAGE_NAME} build not found for platform: ${PAL_PLATFORM_NAME}")
endif()

#add_library(${TARGET_WITH_NAMESPACE} STATIC IMPORTED GLOBAL)

add_library(${TARGET_WITH_NAMESPACE} INTERFACE IMPORTED)

target_link_libraries(${TARGET_WITH_NAMESPACE} 
    INTERFACE
        CMP_Compressonator
)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

set(${LIB_NAME}_FOUND True)
