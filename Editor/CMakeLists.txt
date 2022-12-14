cmake_minimum_required(VERSION 3.20)

project(Editor)

# Editor Modules
add_subdirectory(Source/QtComponents)

add_subdirectory(Source/EditorCore)
add_subdirectory(Source/EditorSystem)
add_subdirectory(Source/CrystalEditor)
add_subdirectory(Source/EditorLauncher)


