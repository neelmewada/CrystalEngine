
if(NOT ${PAL_TRAIT_BUILD_TESTS_SUPPORTED})
    return()
endif()

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


function(ce_add_test NAME)
    set(options AUTORTTI)
    set(oneValueArgs TARGET FOLDER)
    set(multiValueArgs SOURCES)

    cmake_parse_arguments(ce_add_test "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${NAME} ${ce_add_test_SOURCES})

    set_target_properties(${NAME} 
        PROPERTIES
            FOLDER "${ce_add_test_FOLDER}"
    )
    
    target_link_libraries(${NAME}
        PRIVATE
            GTest::GTest
            ${ce_add_test_TARGET}
    )

    if(${PAL_PLATFORM_IS_MAC})
        target_link_libraries(${NAME} PRIVATE "c" "c++")
    endif()

    add_test(${NAME} ${NAME})
    
endfunction()


