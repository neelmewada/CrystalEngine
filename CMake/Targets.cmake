
include_guard(GLOBAL)

ce_set(TARGET_TYPE_SHARED_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_STATIC_IS_LIBRARY TRUE)
#ce_set(TARGET_TYPE_INTERFACE_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_MODULE_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_CONSOLEAPP_IS_EXECUTABLE TRUE)
ce_set(TARGET_TYPE_GUIAPP_IS_EXECUTABLE TRUE)

ce_set(TARGET_TYPE_STATIC_IS_STATICLIB TRUE)
ce_set(TARGET_TYPE_SHARED_IS_SHAREDLIB TRUE)
ce_set(TARGET_TYPE_MODULE_IS_SHAREDLIB TRUE)

# \arg:TARGET_TYPE: SHARED; STATIC; MODULE; CONSOLEAPP ; GUIAPP ;
function(ce_add_target NAME TARGET_TYPE)
    string(TOUPPER ${NAME} NAME_UPPERCASE)

    set(options GENERATED)
    set(oneValueArgs VERSION OUTPUT_SUBDIRECTORY FOLDER)
    set(multiValueArgs PCHHEADER FILES_CMAKE COMPILE_DEFINITIONS INCLUDE_DIRECTORIES BUILD_DEPENDENCIES RUNTIME_DEPENDENCIES)

    cmake_parse_arguments(ce_add_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ce_add_target_FILES_CMAKE)
        message(FATAL_ERROR "No files provided for target ${NAME}") # returns on error
    endif()

    # ------------------------------
    # Configuration
    set(PRIVATE_FILES "")
    set(PUBLIC_FILES "")
    set(CMAKE_FILES "")
    set(INCLUDE_DIRECTORIES "")
    set(EXE_FLAG "")

    # FILES_CMAKE

    set(multiValueArgs PRIVATE PUBLIC)
    cmake_parse_arguments(ce_add_target_FILES_CMAKE "" "" "${multiValueArgs}" ${ce_add_target_FILES_CMAKE})

    foreach(PRIVATE_FILES_CMAKE ${ce_add_target_FILES_CMAKE_PRIVATE})
        include(${PRIVATE_FILES_CMAKE})
        list(APPEND CMAKE_FILES ${PRIVATE_FILES_CMAKE})
        list(APPEND PRIVATE_FILES ${FILES})
    endforeach()

    foreach(PUBLIC_FILES_CMAKE ${ce_add_target_FILES_CMAKE_PUBLIC})
        include(${PUBLIC_FILES_CMAKE})
        list(APPEND CMAKE_FILES ${PUBLIC_FILES_CMAKE})
        list(APPEND PUBLIC_FILES ${FILES})
    endforeach()

    if(${ce_add_target_GENERATED})
        file(GLOB GENERATED_FILES "Generated/*")
    endif()

    # EXE_FLAG

    if(${TARGET_TYPE} STREQUAL "GUIAPP")
        set(EXE_FLAG ${PAL_EXECUTABLE_APPLICATION_FLAG})
    endif()
    
    # Create target

    set(SOURCES ${PRIVATE_FILES})
    list(APPEND SOURCES ${PUBLIC_FILES})
    list(APPEND SOURCES ${GENERATED_FILES})
    list(APPEND SOURCES ${CMAKE_FILES})
    
    if(${TARGET_TYPE_${TARGET_TYPE}_IS_LIBRARY})
        add_library(${NAME} ${TARGET_TYPE} ${SOURCES})
    elseif(${TARGET_TYPE_${TARGET_TYPE}_IS_EXECUTABLE})
        add_executable(${NAME} ${EXE_FLAG} ${SOURCES})
    else()
        message(FATAL_ERROR "Invalid TARGET_TYPE passed: ${TARGET_TYPE}")
    endif()

    source_group(Public FILES ${PUBLIC_FILES})
    source_group(Private FILES ${PRIVATE_FILES})
    source_group(Generated FILES ${GENERATED_FILES})

    # PCH

    if(ce_add_target_PCHHEADER)
        set(multiValueArgs PRIVATE PUBLIC INTERFACE)
        cmake_parse_arguments(ce_add_target_PCHHEADER "" "" "${multiValueArgs}" ${ce_add_target_PCHHEADER})

        target_precompile_headers(${NAME}
            PUBLIC    ${ce_add_target_PCHHEADER_PUBLIC}
            PRIVATE   ${ce_add_target_PCHHEADER_PRIVATE}
            INTERFACE ${ce_add_target_PCHHEADER_INTERFACE}
        )
    endif()

    # FOLDER
    
    if(ce_add_target_FOLDER)
        set_target_properties(${NAME} 
            PROPERTIES
                FOLDER "${ce_add_target_FOLDER}"
        )
    endif()
    
    
    # COMPILE_DEFINITIONS

    set(multiValueArgs PRIVATE PUBLIC INTERFACE)
    cmake_parse_arguments(ce_add_target_COMPILE_DEFINITIONS "" "" "${multiValueArgs}" ${ce_add_target_COMPILE_DEFINITIONS})
    
    if(${TARGET_TYPE_${TARGET_TYPE}_IS_SHAREDLIB})
        if(${PAL_PLATFORM_IS_WINDOWS})
            list(APPEND ce_add_target_COMPILE_DEFINITIONS_PRIVATE "${NAME_UPPERCASE}_API=__declspec(dllexport)")
            list(APPEND ce_add_target_COMPILE_DEFINITIONS_INTERFACE  "${NAME_UPPERCASE}_API=__declspec(dllimport)")
        else()
            list(APPEND ce_add_target_COMPILE_DEFINITIONS_PUBLIC "${NAME_UPPERCASE}_API")
        endif()
    endif()

    target_compile_definitions(${NAME} 
        PRIVATE   ${ce_add_target_COMPILE_DEFINITIONS_PRIVATE} 
        PUBLIC    ${ce_add_target_COMPILE_DEFINITIONS_PUBLIC}
        INTERFACE ${ce_add_target_COMPILE_DEFINITIONS_INTERFACE}
    )

    # INCLUDE_DIRECTORIES

    set(multiValueArgs PRIVATE PUBLIC INTERFACE)
    cmake_parse_arguments(ce_add_target_INCLUDE_DIRECTORIES "" "" "${multiValueArgs}" ${ce_add_target_INCLUDE_DIRECTORIES})

    if(${ce_add_target_GENERATED})
        list(APPEND ce_add_target_INCLUDE_DIRECTORIES_PUBLIC "Generated")
    endif()

    target_include_directories(${NAME}
        PRIVATE   ${ce_add_target_INCLUDE_DIRECTORIES_PRIVATE}
        PUBLIC    ${ce_add_target_INCLUDE_DIRECTORIES_PUBLIC}
        INTERFACE ${ce_add_target_INCLUDE_DIRECTORIES_INTERFACE}
    )
    
    # BUILD_DEPENDENCIES

    set(multiValueArgs PRIVATE PUBLIC INTERFACE)
    cmake_parse_arguments(ce_add_target_BUILD_DEPENDENCIES "" "" "${multiValueArgs}" ${ce_add_target_BUILD_DEPENDENCIES})

    if(ce_add_target_BUILD_DEPENDENCIES_PRIVATE OR ce_add_target_BUILD_DEPENDENCIES_PUBLIC OR ce_add_target_BUILD_DEPENDENCIES_INTERFACE)
        target_link_libraries(${NAME}
            PRIVATE   ${ce_add_target_BUILD_DEPENDENCIES_PRIVATE}
            PUBLIC    ${ce_add_target_BUILD_DEPENDENCIES_PUBLIC}
            INTERFACE ${ce_add_target_BUILD_DEPENDENCIES_INTERFACE}
        )
    endif()
    

endfunction()


