cmake_minimum_required(VERSION 3.20)

project(EditorAssets)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_assets(${PROJECT_NAME}
    TYPE Editor
    ASSET_FILES
        "asset_files.cmake"
    OUTPUT_DIRECTORY "Editor/Assets"
    INCLUDE_DIRECTORIES
        "${CMAKE_SOURCE_DIR}/Engine/Shaders"
    ASSET_EXTENSIONS
        
)

add_dependencies(EngineAssets EditorAssets)
