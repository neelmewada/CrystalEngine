cmake_minimum_required(VERSION 3.20)

project(Tools)

add_dependencies(EditorTools "CE::CrystalEditor")

add_subdirectory(ProjectManager)

add_subdirectory(AssetImporter)
