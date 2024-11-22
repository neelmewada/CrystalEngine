# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::dxcompiler")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "dxcompiler")
set(LIB_NAME "dxcompiler")

set(DXC_COVERAGE OFF CACHE BOOL "" FORCE)
set(HLSL_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)
set(LLVM_APPEND_VC_REV OFF CACHE BOOL "" FORCE)
set(LLVM_BUILD_DOCS OFF CACHE BOOL "" FORCE)

set(LLVM_CODE_COVERAGE_TEST_TARGETS "" CACHE STRING "" FORCE)
set(ENABLE_SPIRV_CODEGEN ON CACHE BOOL "" FORCE)
set(SPIRV_BUILD_TESTS OFF CACHE BOOL "" FORCE)


include(vendor/${PACKAGE_NAME}/cmake/caches/PredefinedParams.cmake)

add_subdirectory(vendor/${PACKAGE_NAME})

add_library(${TARGET_WITH_NAMESPACE} ALIAS dxcompiler)

set_target_properties(dxcompiler PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxcDisassembler PROPERTIES FOLDER "ThirdParty")
set_target_properties(dxc-headers PROPERTIES FOLDER "ThirdParty")
set_target_properties(install-dxc-headers PROPERTIES FOLDER "ThirdParty")
set_target_properties(install-dxc PROPERTIES FOLDER "ThirdParty")
set_target_properties(install-dxcompiler PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxcOptimizer PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilConstants PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilCounters PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilInstructions PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilIntrinsicTables PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilMetadata PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilOperations PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilPIXPasses PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilShaderModel PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilShaderModelInc PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilSigPoint PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilValidation PROPERTIES FOLDER "ThirdParty")
set_target_properties(DxilValidationInc PROPERTIES FOLDER "ThirdParty")
set_target_properties(HCTGen PROPERTIES FOLDER "ThirdParty")
set_target_properties(HLSLIntrinsicOp PROPERTIES FOLDER "ThirdParty")
set_target_properties(HLSLOptions PROPERTIES FOLDER "ThirdParty")
set_target_properties(RDAT_LibraryTypes PROPERTIES FOLDER "ThirdParty")
set_target_properties(install-distribution PROPERTIES FOLDER "ThirdParty")
set_target_properties(extinst_tables PROPERTIES FOLDER "ThirdParty")
set_target_properties(enum_string_mapping PROPERTIES FOLDER "ThirdParty")
set_target_properties(core_tables PROPERTIES FOLDER "ThirdParty")


set(${LIB_NAME}_FOUND True)
