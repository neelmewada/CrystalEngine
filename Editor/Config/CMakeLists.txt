cmake_minimum_required(VERSION 3.20)

project(EditorConfig)

file(GLOB_RECURSE CONFIG_FILES "*.ini")

add_custom_target(${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${CE_OUTPUT_DIR}/Editor/Config
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CE_OUTPUT_DIR}/Editor/Config/CMakeLists.txt
    SOURCES ${CONFIG_FILES}
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        FOLDER "Editor/Config"
)
