
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

macro(windows_add_cmp_lib NAME)
    add_library(${NAME} STATIC IMPORTED)
    set_target_properties(${NAME}
        PROPERTIES
            IMPORTED_LOCATION_DEBUG       ${${LIB_NAME}_LIBS_DIR}/Debug/${NAME}.lib
            IMPORTED_LOCATION_DEVELOPMENT ${${LIB_NAME}_LIBS_DIR}/Development/${NAME}.lib
            IMPORTED_LOCATION_PROFILE     ${${LIB_NAME}_LIBS_DIR}/Development/${NAME}.lib
            IMPORTED_LOCATION_RELEASE     ${${LIB_NAME}_LIBS_DIR}/Release/${NAME}.lib
    )
endmacro()


if (${PAL_PLATFORM_NAME} STREQUAL "Mac" OR ${PAL_PLATFORM_NAME} STREQUAL "Linux")
    set(${LIB_NAME}_STATIC_LIBRARY_DEBUG   ${${LIB_NAME}_LIBS_DIR}/Debug/libCMP_Compressonator.a)
    set(${LIB_NAME}_STATIC_LIBRARY_DEV     ${${LIB_NAME}_LIBS_DIR}/Development/libCMP_Compressonator.a)
    set(${LIB_NAME}_STATIC_LIBRARY_RELEASE ${${LIB_NAME}_LIBS_DIR}/Release/libCMP_Compressonator.a)

    set_target_properties(${TARGET_WITH_NAMESPACE}
        PROPERTIES
            IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${LIB_NAME}.a"
    )

elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows")
    windows_add_cmp_lib(CMP_Compressonator)
    windows_add_cmp_lib(CMP_Core_AVX)
    windows_add_cmp_lib(CMP_Core_AVX512)
    windows_add_cmp_lib(CMP_Core_SSE)
    windows_add_cmp_lib(CMP_Core)
    windows_add_cmp_lib(CMP_Common)
    windows_add_cmp_lib(CMP_Framework)
    windows_add_cmp_lib(CMP_GpuDecode)
    windows_add_cmp_lib(GPUDecode_DirectX)
    windows_add_cmp_lib(GPUDecode_Vulkan)
    windows_add_cmp_lib(Image_BRLG)
    windows_add_cmp_lib(EncodeWith_GPU)
    windows_add_cmp_lib(brotlig)
    windows_add_cmp_lib(brotli)
    set(GPU_DECODE_LIBS GPUDecode_DirectX GPUDecode_Vulkan)

else()
    error("${PACKAGE_NAME} build not found for platform: ${PAL_PLATFORM_NAME}")
endif()

#add_library(${TARGET_WITH_NAMESPACE} STATIC IMPORTED GLOBAL)

add_library(${TARGET_WITH_NAMESPACE} INTERFACE IMPORTED)

target_link_libraries(${TARGET_WITH_NAMESPACE} 
    INTERFACE
        CMP_Compressonator
        CMP_Core_AVX
        CMP_Core_AVX512
        CMP_Core_SSE
        CMP_Core
        CMP_Framework
        CMP_GpuDecode
        CMP_Common
        ${GPU_DECODE_LIBS}
        Image_BRLG
        EncodeWith_GPU
        brotlig
        brotli
)

target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

set(${LIB_NAME}_FOUND True)
