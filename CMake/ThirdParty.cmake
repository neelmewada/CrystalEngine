
include_guard(GLOBAL)

# file(GLOB THIRD_PARTY_CMAKE_FILES ${CE_ROOT_DIR}/ThirdParty/*${PAL_PLATFORM_NAME_LOWERCASE}*/Find*.cmake)

# foreach(THIRD_PARTY_CMAKE_FILE ${THIRD_PARTY_CMAKE_FILES})
#     get_filename_component(THIRD_PARTY_CMAKE_FILE_DIRECTORY ${THIRD_PARTY_CMAKE_FILE} DIRECTORY)
#     list(APPEND CMAKE_MODULE_PATH ${THIRD_PARTY_CMAKE_FILE_DIRECTORY})
# endforeach()

list(APPEND CMAKE_MODULE_PATH ${CE_ROOT_DIR}/ThirdParty)

# \arg:TARGET_TYPE: SHARED; STATIC; MODULE; 
function(ce_add_external_target NAME TARGET_TYPE)
    string(TOUPPER ${NAME} NAME_UPPERCASE)

    set(options GENERATED)
    set(oneValueArgs VERSION PCHHEADER OUTPUT_SUBDIRECTORY)
    set(multiValueArgs FILES_CMAKE COMPILE_DEFINITIONS INCLUDE_DIRECTORIES BUILD_DEPENDENCIES RUNTIME_DEPENDENCIES)

    cmake_parse_arguments(ce_add_external_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

endfunction()

find_package(Qt6 REQUIRED COMPONENTS Core)

