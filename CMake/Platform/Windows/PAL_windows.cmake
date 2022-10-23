

ce_set(PAL_EXECUTABLE_APPLICATION_FLAG WIN32)

if(${CE_STANDALONE})
    ce_set(PAL_TRAIT_BUILD_EDITOR FALSE)
    ce_set(PAL_TRAIT_BUILD_HOST_TOOLS FALSE)
    ce_set(PAL_TRAIT_BUILD_HOST_LIBRARIES FALSE)
else()
    ce_set(PAL_TRAIT_BUILD_EDITOR TRUE)
    ce_set(PAL_TRAIT_BUILD_HOST_TOOLS TRUE)
    ce_set(PAL_TRAIT_BUILD_HOST_LIBRARIES TRUE)
endif()

ce_set(PAL_TRAIT_BUILD_EDITOR_TESTS_SUPPORTED TRUE)
ce_set(PAL_TRAIT_BUILD_ENGINE_TESTS_SUPPORTED TRUE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    ce_set(PAL_TRAIT_COMPILER_ID MSVC)
    ce_set(PAL_TRAIT_COMPILER_ID_LOWERCASE msvc)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    ce_set(PAL_TRAIT_COMPILER_ID Clang)
    ce_set(PAL_TRAIT_COMPILER_ID_LOWERCASE clang)
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} not supported on ${PAL_PLATFORM_NAME}")
endif()

add_compile_definitions(PLATFORM_WINDOWS=1)
ce_set(PAL_PLATFORM_IS_WINDOWS TRUE)

