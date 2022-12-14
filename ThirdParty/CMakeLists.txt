cmake_minimum_required(VERSION 3.20)

project(ThirdParty)

# Utils
find_package(xxHash REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml REQUIRED)

# SDL2
find_package(sdl REQUIRED)

include(FindQt.cmake)
find_package(qads REQUIRED)
