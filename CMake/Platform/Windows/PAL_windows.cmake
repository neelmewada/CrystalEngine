

ce_set(PAL_EXECUTABLE_APPLICATION_FLAG WIN32_EXECUTABLE)

if(${CE_STANDALONE})
    ce_set(PAL_TRAIT_BUILD_EDITOR FALSE)
    ce_set(PAL_TRAIT_BUILD_HOST_TOOLS FALSE)
    ce_set(PAL_TRAIT_BUILD_HOST_LIBRARIES FALSE)
else()
    ce_set(PAL_TRAIT_BUILD_EDITOR TRUE)
    ce_set(PAL_TRAIT_BUILD_HOST_TOOLS TRUE)
    ce_set(PAL_TRAIT_BUILD_HOST_LIBRARIES TRUE)
    add_compile_definitions(PAL_TRAIT_WITH_EDITOR=1)
endif()

# Graphics APIs
ce_set(PAL_TRAIT_VULKAN_SUPPORTED TRUE)
ce_set(PAL_TRAIT_METAL_SUPPORTED FALSE)

add_compile_definitions(PAL_TRAIT_VULKAN_SUPPORTED=1)


ce_set(PAL_TRAIT_BUILD_EDITOR_TESTS_SUPPORTED TRUE)
ce_set(PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED TRUE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    ce_set(PAL_TRAIT_COMPILER_ID MSVC)
    ce_set(PAL_TRAIT_COMPILER_ID_LOWERCASE msvc)
    add_compile_definitions(COMPILER_MSVC=1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    ce_set(PAL_TRAIT_COMPILER_ID Clang)
    ce_set(PAL_TRAIT_COMPILER_ID_LOWERCASE clang)
    add_compile_definitions(COMPILER_CLANG=1)
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} not supported on ${PAL_PLATFORM_NAME}")
endif()

ce_set(PAL_PLATFORM_IS_64_BIT TRUE)
ce_set(PAL_PLATFORM_IS_WINDOWS TRUE)

add_compile_definitions(PLATFORM_WINDOWS=1)
add_compile_definitions(IS_64BIT=1)
