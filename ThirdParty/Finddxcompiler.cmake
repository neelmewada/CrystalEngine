# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::dxcompiler")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "dxcompiler")
set(LIB_NAME "dxcompiler")

set(HLSL_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)

set(LLVM_CODE_COVERAGE_TEST_TARGETS "" CACHE STRING "" FORCE)
set(ENABLE_SPIRV_CODEGEN ON CACHE BOOL "" FORCE)


include(vendor/${PACKAGE_NAME}/cmake/caches/PredefinedParams.cmake)

add_subdirectory(vendor/${PACKAGE_NAME})

add_library(${TARGET_WITH_NAMESPACE} ALIAS dxcompiler)

set_target_properties(dxcompiler PROPERTIES FOLDER "ThirdParty")


set(${LIB_NAME}_FOUND True)
