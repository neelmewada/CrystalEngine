
include_guard(GLOBAL)

# file(GLOB THIRD_PARTY_CMAKE_FILES ${CE_ROOT_DIR}/ThirdParty/*${PAL_PLATFORM_NAME_LOWERCASE}*/Find*.cmake)

# foreach(THIRD_PARTY_CMAKE_FILE ${THIRD_PARTY_CMAKE_FILES})
#     get_filename_component(THIRD_PARTY_CMAKE_FILE_DIRECTORY ${THIRD_PARTY_CMAKE_FILE} DIRECTORY)
#     list(APPEND CMAKE_MODULE_PATH ${THIRD_PARTY_CMAKE_FILE_DIRECTORY})
# endforeach()

list(APPEND CMAKE_MODULE_PATH ${CE_ROOT_DIR}/ThirdParty)

set(FETCHCONTENT_QUIET FALSE)

# \arg:TARGET_TYPE: SHARED; STATIC; MODULE; 
function(ce_validate_package PACKAGE_NAME PACKAGE_SHORT_NAME)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_NAME}")
        return()
    endif()
    
    set(package_url "https://gitlab.com/neelrmewada/crystalengine-thirdparty/-/raw/main/ThirdParty/${PAL_PLATFORM_NAME}/${PACKAGE_NAME}.zip?inline=false")
    set(package_local_zip_path "${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_NAME}.zip")

    if(NOT EXISTS "${package_local_zip_path}")
        message("***********************************************************************************")

        message(STATUS "Downloading Package: ${PACKAGE_SHORT_NAME}")

        file(DOWNLOAD ${package_url} "${package_local_zip_path}" STATUS download_status SHOW_PROGRESS)
        
        list(GET download_status 0 status_code)
        list(GET download_status 1 status_code_msg)
        
        if(NOT ${status_code} EQUAL 0)
            message(SEND_ERROR "Download Failed! Error code ${status_code}. Error Message: ${status_code_msg}\nDownload URL: ${package_url}")
            file(REMOVE ${package_local_zip_path})
            return()
        endif()
        
        file(ARCHIVE_EXTRACT INPUT ${package_local_zip_path} DESTINATION "${CMAKE_CURRENT_LIST_DIR}" VERBOSE)
        file(REMOVE ${package_local_zip_path})

        message("***********************************************************************************")
    endif()

endfunction()

include("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/FindQt.cmake")

# set(QT6_COMPONENTS
#     Core
#     Concurrent
#     Gui
#     Svg
#     Widgets
#     Xml
# )

find_package(Qt6 REQUIRED COMPONENTS ${QT6_COMPONENTS})

