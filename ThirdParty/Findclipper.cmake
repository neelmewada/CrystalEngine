
# this file actually ingests the library and defines targets.
set(TARGET_WITH_NAMESPACE "ThirdParty::clipper")
if (TARGET ${TARGET_WITH_NAMESPACE})
    return()
endif()

set(PACKAGE_NAME "Clipper2")
set(LIB_NAME "clipper")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CLIPPER2_TESTS OFF CACHE BOOL "" FORCE)
set(CLIPPER2_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/${PACKAGE_NAME}/CPP)

add_library(${TARGET_WITH_NAMESPACE} ALIAS Clipper2)

set_target_properties(Clipper2 PROPERTIES FOLDER "ThirdParty")


