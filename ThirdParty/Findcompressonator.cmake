
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::compressonator")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "compressonator")
set(PACKAGE_VERISON "4.5.52")
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
            IMPORTED_LOCATION_DEVELOPMENT ${${LIB_NAME}_LIBS_DIR}/Release/${NAME}.lib
            IMPORTED_LOCATION_PROFILE     ${${LIB_NAME}_LIBS_DIR}/Release/${NAME}.lib
            IMPORTED_LOCATION_RELEASE     ${${LIB_NAME}_LIBS_DIR}/Release/${NAME}.lib
    )
endmacro()

macro(mac_add_cmp_lib NAME)
    add_library(${NAME} SHARED IMPORTED)
    set_target_properties(${NAME}
        PROPERTIES
            IMPORTED_LOCATION_DEBUG       ${${LIB_NAME}_LIBS_DIR}/Debug/lib${NAME}.dylib
            IMPORTED_LOCATION_DEVELOPMENT ${${LIB_NAME}_LIBS_DIR}/Release/lib${NAME}.dylib
            IMPORTED_LOCATION_PROFILE     ${${LIB_NAME}_LIBS_DIR}/Release/lib${NAME}.dylib
            IMPORTED_LOCATION_RELEASE     ${${LIB_NAME}_LIBS_DIR}/Release/lib${NAME}.dylib
            #IMPORTED_LOCATION "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Development,$<CONFIG>>/lib${NAME}.dylib"
    )
endmacro()


add_library(${TARGET_WITH_NAMESPACE} INTERFACE IMPORTED)

if (${PAL_PLATFORM_NAME} STREQUAL "Mac") # Mac
    mac_add_cmp_lib(CMP_Compressonator)
    mac_add_cmp_lib(CMP_Core)
    mac_add_cmp_lib(CMP_Framework)

    target_link_libraries(${TARGET_WITH_NAMESPACE}
        INTERFACE
            CMP_Compressonator
            CMP_Core
            CMP_Framework
    )

    ce_add_rt_deps(compressonator
        ROOT_PATH "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Release,$<CONFIG>>"
        COPY_FILES_MAC
            libCMP_Compressonator.dylib
            libCMP_Core.dylib
            libCMP_Framework.dylib
    )

elseif (${PAL_PLATFORM_NAME} STREQUAL "Windows") # Windows
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
    windows_add_cmp_lib(EncodeWith_GPU)
    set(GPU_DECODE_LIBS GPUDecode_DirectX GPUDecode_Vulkan)

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
            EncodeWith_GPU
    )

    ce_add_rt_deps(compressonator
        ROOT_PATH "${${LIB_NAME}_LIBS_DIR}/$<IF:$<CONFIG:Development,Profile>,Release,$<CONFIG>>"
        COPY_LIBS
            EncodeWith_DXC
            EncodeWith_GPU
            EncodeWith_OCL
    )

else()
    error("${PACKAGE_NAME} build not found for platform: ${PAL_PLATFORM_NAME}")
endif()


target_include_directories(${TARGET_WITH_NAMESPACE}
    INTERFACE
        ${${LIB_NAME}_INCLUDE_DIR}
)

set(${LIB_NAME}_FOUND True)
