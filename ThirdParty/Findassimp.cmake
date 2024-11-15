
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::assimp")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "assimp")

add_subdirectory(vendor/${PACKAGE_NAME})

add_library(${TARGET_WITH_NAMESPACE} ALIAS assimp)

set_target_properties(assimp PROPERTIES FOLDER "ThirdParty")

