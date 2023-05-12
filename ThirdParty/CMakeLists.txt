cmake_minimum_required(VERSION 3.20)

project(ThirdParty)


# Qt
if(${PAL_TRAIT_QT_SUPPORTED})
    include(FindQt.cmake)
    find_package(qads REQUIRED)
    find_package(qtnodes REQUIRED)
endif()


