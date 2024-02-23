cmake_minimum_required(VERSION 3.20)

project(EngineAssets)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)

ce_add_assets(${PROJECT_NAME}
    TYPE Engine
    ASSET_FILES
        "asset_files.cmake"
    OUTPUT_DIRECTORY "Engine/Assets"
    INCLUDE_DIRECTORIES
        "${CMAKE_SOURCE_DIR}/Engine/Shaders"
    ASSET_EXTENSIONS
        ".shader"
        ".png"
        ".jpg"
        ".jpeg"
)
