
include_guard(GLOBAL)

set(CE_CONFIGURATION_TYPES "Debug;Development;Profile;Release" CACHE STRING "" FORCE)

include(CMake/ConfigurationTypes.cmake)

add_compile_definitions("CE_DEBUG=$<CONFIG:Debug>")
add_compile_definitions("CE_BUILD_$<UPPER_CASE:$<CONFIG>>=1")

add_compile_definitions("CE_NAME_DEBUG=$<CONFIG:Debug,Development>")
