cmake_minimum_required(VERSION 3.20)

project(EngineResources)

file(GLOB_RECURSE RESOURCE_FILES "*.ttf")

ce_add_resources(${PROJECT_NAME}
    RESOURCES
        ${RESOURCE_FILES}
    FOLDER "Resources"
    OUTPUT_DIRECTORY "Engine/Resources"
)
