cmake_minimum_required(VERSION 3.20)

project(Editor)
set(CURRENT_PACKAGE_CATEGORY "Editor")

# Assets
add_subdirectory(Assets)

# Editor Modules
add_subdirectory(Source/EditorCore)
add_subdirectory(Source/EditorWidgets)
add_subdirectory(Source/EditorSystem)
#add_subdirectory(Source/CrystalEditor)

# Editor Executables
#add_subdirectory(Source/ProjectBrowser)
#add_subdirectory(Source/EditorLauncher)
