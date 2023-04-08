enable_testing()

set(TEST_TARGET System)
set(TEST_NAME ${TEST_TARGET}_Test)

project(${TEST_NAME})

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_executable(${TEST_NAME} ${SOURCES})

set_target_properties(${TEST_NAME} 
    PROPERTIES
        FOLDER "Tests/Engine"
)

target_link_libraries(${TEST_NAME} PRIVATE ${TEST_TARGET})

if(${PAL_PLATFORM_IS_MAC})
    target_link_libraries(${TEST_NAME} PRIVATE "c" "c++")
endif()


add_test(NAME ${TEST_NAME}
    COMMAND ${TEST_NAME}
)
