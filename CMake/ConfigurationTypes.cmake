
include_guard(GLOBAL)

set(CMAKE_CONFIGURATION_TYPES ${CE_CONFIGURATION_TYPES} CACHE STRING "" FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_DEVELOPMENT ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} CACHE STRING "")
set(CMAKE_SHARED_LINKER_FLAGS_PROFILE ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} CACHE STRING "")

