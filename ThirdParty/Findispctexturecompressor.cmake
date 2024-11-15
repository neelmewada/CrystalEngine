
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::ispctexturecompressor")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "ispctexturecompressor")
set(LIB_NAME "ispc_texcomp")

add_subdirectory(vendor/${PACKAGE_NAME})

add_library(${TARGET_WITH_NAMESPACE} ALIAS ispc_texcomp)

set_target_properties(ispc_texcomp PROPERTIES FOLDER "ThirdParty")

