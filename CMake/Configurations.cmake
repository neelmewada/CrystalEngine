
include_guard(GLOBAL)

set(CE_CONFIGURATION_TYPES "Debug;Development;Profile;Release" CACHE STRING "" FORCE)

include(CMake/ConfigurationTypes.cmake)

add_compile_definitions("DEBUG=$<CONFIG:Debug>")
add_compile_definitions("CE_BUILD_TYPE=\"$<CONFIG>\"")

add_compile_definitions("CE_BUILD_DEBUG=$<CONFIG:Debug>")
add_compile_definitions("CE_BUILD_DEVELOPMENT=$<CONFIG:Development>")
add_compile_definitions("CE_BUILD_PROFILE=$<CONFIG:Profile>")
add_compile_definitions("CE_BUILD_RELEASE=$<CONFIG:Release>")
