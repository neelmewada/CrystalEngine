cmake_minimum_required(VERSION 3.20)

set(TEST_TARGET Sandbox)

set(TEST_NAME ${TEST_TARGET}_Test)
project(${TEST_NAME})

enable_testing()

file(GLOB_RECURSE SRCS "*.cpp" "*.h")

ce_add_test(${PROJECT_NAME}
    TARGET Sandbox
    FOLDER "Tests/Engine"
    SOURCES
        ${SRCS}
    BUILD_DEPENDENCIES
        TARGETS Config
)
