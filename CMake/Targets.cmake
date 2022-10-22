
include_guard(GLOBAL)

ce_set(TARGET_TYPE_SHARED_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_STATIC_IS_LIBRARY TRUE)
#ce_set(TARGET_TYPE_INTERFACE_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_MODULE_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_CONSOLEAPP_IS_EXECUTABLE TRUE)
ce_set(TARGET_TYPE_GUIAPP_IS_EXECUTABLE TRUE)

# \arg:TARGET_TYPE: SHARED; STATIC; MODULE; CONSOLEAPP ; GUIAPP ;
function(ce_add_target NAME TARGET_TYPE)
    set(options REFLECT GENERATED)
    set(oneValueArgs VERSION PACKAGE OUTPUT_SUBDIRECTORY)
    set(multiValueArgs FILES_CMAKE COMPILE_DEFINITIONS INCLUDE_DIRECTORIES BUILD_DEPENDENCIES RUNTIME_DEPENDENCIES)

    cmake_parse_arguments(ce_add_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ce_add_target_FILES_CMAKE)
        message(FATAL_ERROR "No files provided for target ${NAME}") # returns on error
    endif()

    # ------------------------------
    # Configuration
    set(PRIVATE_FILES "")
    set(PUBLIC_FILES "")
    set(EXTRA_FILES "")
    set(INCLUDE_DIRECTORIES "")
    set(EXE_FLAG "")

    # FILES_CMAKE

    set(multiValueArgs PRIVATE PUBLIC)
    cmake_parse_arguments(ce_add_target_FILES_CMAKE "" "" "${multiValueArgs}" ${ce_add_target_FILES_CMAKE})

    foreach(PRIVATE_FILES_CMAKE ${ce_add_target_FILES_CMAKE_PRIVATE})
        include(${PRIVATE_FILES_CMAKE})
        list(APPEND EXTRA_FILES ${PRIVATE_FILES_CMAKE})
        list(APPEND PRIVATE_FILES ${FILES})
    endforeach()

    foreach(PUBLIC_FILES_CMAKE ${ce_add_target_FILES_CMAKE_PUBLIC})
        include(${PUBLIC_FILES_CMAKE})
        list(APPEND EXTRA_FILES ${PUBLIC_FILES_CMAKE})
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
    
    if(${TARGET_TYPE_${TARGET_TYPE}_IS_LIBRARY})
        add_library(${NAME} ${TARGET_TYPE} "${PRIVATE_FILES};${PUBLIC_FILES};${GENERATED_FILES};${EXTRA_FILES}")
    elseif(${TARGET_TYPE_${TARGET_TYPE}_IS_EXECUTABLE})
        add_executable(${NAME} ${EXE_FLAG} "${PRIVATE_FILES};${PUBLIC_FILES};${GENERATED_FILES};${EXTRA_FILES}")
    else()
        message(FATAL_ERROR "Invalid TARGET_TYPE passed: ${TARGET_TYPE}")
    endif()

    source_group(Public FILES ${PUBLIC_FILES})
    source_group(Private FILES ${PRIVATE_FILES})
    source_group(Generated FILES ${GENERATED_FILES})
    
    # COMPILE_DEFINITIONS

    set(multiValueArgs PRIVATE PUBLIC INTERFACE)
    cmake_parse_arguments(ce_add_target_COMPILE_DEFINITIONS "" "" "${multiValueArgs}" ${ce_add_target_COMPILE_DEFINITIONS})

    if(${ce_add_target_GENERATED})
        list(APPEND ce_add_target_COMPILE_DEFINITIONS_PUBLIC "Generated")
    endif()

    target_compile_definitions(${NAME} 
        PRIVATE   ${ce_add_target_COMPILE_DEFINITIONS_PRIVATE} 
        PUBLIC    ${ce_add_target_COMPILE_DEFINITIONS_PUBLIC}
        INTERFACE ${ce_add_target_COMPILE_DEFINITIONS_INTERFACE}
    )

    # INCLUDE_DIRECTORIES

    set(multiValueArgs PRIVATE PUBLIC INTERFACE)
    cmake_parse_arguments(ce_add_target_INCLUDE_DIRECTORIES "" "" "${multiValueArgs}" ${ce_add_target_INCLUDE_DIRECTORIES})

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


