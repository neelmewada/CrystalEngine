cmake_minimum_required(VERSION 3.20)

project(EditorStyles)

file(GLOB_RECURSE CSS_FILES "*.css")

ce_add_styles(${PROJECT_NAME}
    STYLES
        ${CSS_FILES}
    FOLDER "Styles"
    OUTPUT_DIRECTORY "Editor/Styles"
)
