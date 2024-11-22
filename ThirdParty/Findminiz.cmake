
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::miniz")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "miniz")
set(LIB_NAME "miniz")

add_subdirectory(vendor/${PACKAGE_NAME})

add_library(${TARGET_WITH_NAMESPACE} ALIAS miniz)

set_target_properties(miniz PROPERTIES FOLDER "ThirdParty")


