
set(TARGET_WITH_NAMESPACE "ThirdParty::cxxopts")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "cxxopts")
set(PACKAGE_VERISON "3.0.0")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

ce_validate_package(${PACKAGE_FULL_NAME} ${PACKAGE_NAME})

set(${PACKAGE_NAME}_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME})

set(${PACKAGE_NAME}_INCLUDE_DIR ${${PACKAGE_NAME}_BASE_DIR}/${PACKAGE_NAME}/include)

add_library(${TARGET_WITH_NAMESPACE} INTERFACE IMPORTED GLOBAL)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${PACKAGE_NAME}_INCLUDE_DIR}
)

set(${PACKAGE_NAME}_FOUND True)
