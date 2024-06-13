cmake_minimum_required(VERSION 3.20)

project(EngineShaders CXX)

file(GLOB_RECURSE HLSL_FILES "*.hlsl" "*.hlsli")

ce_add_shaders(${PROJECT_NAME}
    SOURCES
        "${HLSL_FILES}"
    FOLDER
        "Engine/Shaders"
    OUTPUT_DIRECTORY
        "Engine/Shaders"
)

