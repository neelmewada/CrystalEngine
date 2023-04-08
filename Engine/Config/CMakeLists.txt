cmake_minimum_required(VERSION 3.20)

project(EngineConfig)

file(GLOB_RECURSE CONFIG_FILES "*.ini")

add_custom_target(${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${CE_OUTPUT_DIR}/Engine/Config
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CE_OUTPUT_DIR}/Engine/Config/CMakeLists.txt
    SOURCES ${CONFIG_FILES}
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        FOLDER "Engine/Config"
)
