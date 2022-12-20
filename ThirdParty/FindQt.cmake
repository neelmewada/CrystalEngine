

set(PACKAGE_NAME "qt")
set(PACKAGE_VERISON "6.4.1")
set(PACKAGE_REVISION 1)

set(PACKAGE_FULL_NAME "${PACKAGE_NAME}-${PACKAGE_VERISON}-rev${PACKAGE_REVISION}-${PAL_PLATFORM_NAME_LOWERCASE}")

set(QT_PACKAGE_NAME qt)

set(QT_PATH "${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_FULL_NAME}/qt" CACHE STRING "The root path to Qt" FORCE)
mark_as_advanced(QT_PATH)
if(NOT EXISTS ${QT_PATH})
    message(FATAL_ERROR "Cannot find ThirdParty library ${QT_PACKAGE_NAME} on path ${QT_PATH}")
endif()

set(${QT_PACKAGE_NAME}_BIN_DIR ${QT_PATH}/bin PARENT_SCOPE)

# Force-set QtCore's version here to ensure CMake detects Qt's existence and allows AUTOMOC to work
set(Qt6Core_VERSION_MAJOR "6" CACHE STRING "Qt's major version" FORCE)
set(Qt6Core_VERSION_MINOR "4" CACHE STRING "Qt's minor version" FORCE)
set(Qt6Core_VERSION_PATCH "1" CACHE STRING "Qt's patch version" FORCE)
mark_as_advanced(Qt6Core_VERSION_MAJOR)
mark_as_advanced(Qt6Core_VERSION_MINOR)
mark_as_advanced(Qt6Core_VERSION_PATCH)

set(QT6_COMPONENTS
    Core
    Concurrent
    Gui
    Network
    Svg
    Widgets
    Xml
)

set(QT_LIB_PATH ${QT_PATH}/lib)

list(APPEND CMAKE_PREFIX_PATH ${QT_LIB_PATH}/cmake/Qt6)

if(${PAL_PLATFORM_IS_WINDOWS}) # Windows
    set(${QT_PACKAGE_NAME}_RUNTIME_DEPS "Qt6Multimedia$<$<CONFIG:DEBUG>:d>.dll")
    foreach(component ${QT6_COMPONENTS})
        list(APPEND ${QT_PACKAGE_NAME}_RUNTIME_DEPS "Qt6${component}$<$<CONFIG:DEBUG>:d>.dll")
    endforeach()

    set(${QT_PACKAGE_NAME}_RUNTIME_DEPS ${${QT_PACKAGE_NAME}_RUNTIME_DEPS} PARENT_SCOPE)
else(${PAL_PLATFORM_IS_MAC})
    set(${QT_PACKAGE_NAME}_RUNTIME_DEPS_FRAMEWORKS "QtMultimedia.framework")
    foreach(component ${QT6_COMPONENTS})
        list(APPEND ${QT_PACKAGE_NAME}_RUNTIME_DEPS_FRAMEWORKS "Qt${component}.framework")
    endforeach()
endif()

set(QT_ADDITIONAL_INCLUDES
    "${QT_PATH}/include/QtWidgets/${PACKAGE_VERISON}/"
    "${QT_PATH}/include/QtGui/${PACKAGE_VERISON}/"
    "${QT_PATH}/include/QtCore/${PACKAGE_VERISON}/"
    "${QT_PATH}/include/QtWidgets/${PACKAGE_VERISON}/QtWidgets"
    "${QT_PATH}/include/QtGui/${PACKAGE_VERISON}/QtGui"
    "${QT_PATH}/include/QtCore/${PACKAGE_VERISON}/QtCore"
PARENT_SCOPE)


