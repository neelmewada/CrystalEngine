
include_guard(GLOBAL)

if(${PAL_PLATFORM_IS_WINDOWS})
    add_compile_options(/w44003)
endif()
