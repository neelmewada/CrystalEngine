cmake_minimum_required(VERSION 3.20)

set(TEST_TARGET Core)

set(TEST_NAME ${TEST_TARGET}_Test)
project(${TEST_NAME})

enable_testing()

ce_add_test(${PROJECT_NAME}
    TARGET Core
    FOLDER "Tests/Engine"
    SOURCES
        "Main.cpp"
)
