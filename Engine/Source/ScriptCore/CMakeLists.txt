cmake_minimum_required(VERSION 3.20)

project(ScriptCore LANGUAGES CSharp)

include(CSharpUtilities)

include(scriptcore_source_files.cmake)

add_library(${PROJECT_NAME}
    ${FILES}
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        FOLDER "Engine/Source"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.8")

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
    "Microsoft.CSharp"
    "System"
    "System.Core"
)
