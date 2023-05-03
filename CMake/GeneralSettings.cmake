
include_guard(GLOBAL)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
ce_set(CMAKE_WARN_DEPRECATED ON)

set(CE_EXTERNAL_SUBDIRS "" CACHE STRING "Additional list of subdirectory to recurse into via the cmake `add_subdirectory()` command. \
    The subdirectories are included after the restricted platform folders have been visited by a call to `add_subdirectory(restricted/\${restricted_platform})`")

ce_set(CE_ROOT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
