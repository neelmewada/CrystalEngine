
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

function(ce_group_sources_by_folder target)
  set(SOURCE_GROUP_DELIMITER "/")
  set(last_dir "")
  set(files "")

  get_target_property(sources ${target} SOURCES)

  foreach(file ${sources})
    if(IS_ABSOLUTE ${file})
        file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}" ${file})
    else()
        set(relative_file ${file})
    endif()
    
    get_filename_component(dir "${relative_file}" PATH)
    if(NOT "${dir}" STREQUAL "${last_dir}")
      if(files)
        source_group("${last_dir}" FILES ${files})
      endif()
      set(files "")
    endif()
    set(files ${files} ${file})
    set(last_dir "${dir}")
  endforeach()

  if(files)
    source_group("${last_dir}" FILES ${files})
  endif()
endfunction()

# \arg:TARGET_TYPE: SHARED; STATIC; MODULE; CONSOLEAPP ; GUIAPP ;
function(ce_add_target NAME TARGET_TYPE)
    string(TOUPPER ${NAME} NAME_UPPERCASE)

    set(options GENERATED AUTOMOC AUTOUIC AUTORCC COPY_CONFIGS)
    set(oneValueArgs VERSION OUTPUT_SUBDIRECTORY FOLDER NAMESPACE OUTPUT_DIRECTORY)
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
        #set(EXE_FLAG $<$<CONFIG:Release>:${PAL_EXECUTABLE_APPLICATION_FLAG}>)
    endif()
    
    # Create target

    set(SOURCES ${PRIVATE_FILES})
    list(APPEND SOURCES ${PUBLIC_FILES})
    list(APPEND SOURCES ${GENERATED_FILES})
    list(APPEND SOURCES ${CMAKE_FILES})
    
    if(${TARGET_TYPE_${TARGET_TYPE}_IS_LIBRARY})
        add_library(${NAME} ${TARGET_TYPE} ${SOURCES})

        if(ce_add_target_NAMESPACE)
            add_library("${ce_add_target_NAMESPACE}::${NAME}" ALIAS ${NAME})
        endif()

    elseif(${TARGET_TYPE_${TARGET_TYPE}_IS_EXECUTABLE})
        add_executable(${NAME} ${EXE_FLAG} ${SOURCES})

        if(${TARGET_TYPE} STREQUAL "GUIAPP")
            set_target_properties(${NAME} 
                PROPERTIES ${PAL_EXECUTABLE_APPLICATION_FLAG} $<CONFIG:Release>
            )
        endif()
        
    else()
        message(FATAL_ERROR "Invalid TARGET_TYPE passed: ${TARGET_TYPE}")
    endif()
    
    ce_group_sources_by_folder(${NAME})

    # source_group(Public FILES ${PUBLIC_FILES})
    # source_group(Private FILES ${PRIVATE_FILES})
    # source_group(Generated FILES ${GENERATED_FILES})

    # OUTPUT_DIRECTORY

    if(ce_add_target_OUTPUT_DIRECTORY)
        # set_target_properties(${NAME}
        #     PROPERTIES
        #         ARCHIVE_OUTPUT_DIRECTORY "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
        #         LIBRARY_OUTPUT_DIRECTORY "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
        #         RUNTIME_OUTPUT_DIRECTORY "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
        # )
    endif()

    # COPY_CONFIGS

    if(ce_add_target_COPY_CONFIGS)
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../Config ${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}/Config
        )
    endif()
    
    
    # Qt AUTO*

    if(ce_add_target_AUTOMOC)
        set_target_properties(${NAME} 
            PROPERTIES AUTOMOC ON
        )
    endif()
    if(ce_add_target_AUTOUIC)
        set_target_properties(${NAME} 
            PROPERTIES AUTOUIC ON
        )
    endif()
    if(ce_add_target_AUTORCC)
        set_target_properties(${NAME} 
            PROPERTIES AUTORCC ON
        )
    endif()
    

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
            list(APPEND ce_add_target_COMPILE_DEFINITIONS_PUBLIC "${NAME_UPPERCASE}_API=")
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

    set(multiValueArgs PRIVATE PUBLIC INTERFACE TARGETS)
    cmake_parse_arguments(ce_add_target_BUILD_DEPENDENCIES "" "" "${multiValueArgs}" ${ce_add_target_BUILD_DEPENDENCIES})

    if(${PAL_PLATFORM_IS_MAC})
        list(APPEND ce_add_target_BUILD_DEPENDENCIES_PRIVATE "c")
        list(APPEND ce_add_target_BUILD_DEPENDENCIES_PRIVATE "c++")
        list(APPEND ce_add_target_BUILD_DEPENDENCIES_PRIVATE "-framework CoreServices")
    endif()
    

    if(ce_add_target_BUILD_DEPENDENCIES_PRIVATE OR ce_add_target_BUILD_DEPENDENCIES_PUBLIC OR ce_add_target_BUILD_DEPENDENCIES_INTERFACE)
        target_link_libraries(${NAME}
            PRIVATE   ${ce_add_target_BUILD_DEPENDENCIES_PRIVATE}
            PUBLIC    ${ce_add_target_BUILD_DEPENDENCIES_PUBLIC}
            INTERFACE ${ce_add_target_BUILD_DEPENDENCIES_INTERFACE}
        )
    endif()

    if(ce_add_target_BUILD_DEPENDENCIES_TARGETS)
        add_dependencies(${NAME} ${ce_add_target_BUILD_DEPENDENCIES_TARGETS})
    endif()
    
    
    # RUNTIME_DEPENDENCIES

    foreach(runtime_dep ${ce_add_target_RUNTIME_DEPENDENCIES})
        if(DEFINED ${runtime_dep}_BIN_DIR AND DEFINED ${runtime_dep}_RUNTIME_DEPS)
            foreach(copy_dll ${${runtime_dep}_RUNTIME_DEPS})
                if(${TARGET_TYPE_${TARGET_TYPE}_IS_LIBRARY})
                    add_custom_command(TARGET ${NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${runtime_dep}_BIN_DIR}/${copy_dll} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
                    )
                elseif(${TARGET_TYPE_${TARGET_TYPE}_IS_EXECUTABLE})
                    add_custom_command(TARGET ${NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${runtime_dep}_BIN_DIR}/${copy_dll} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
                    )
                endif()
            endforeach()
        endif()
        # Mac frameworks
        if(DEFINED ${runtime_dep}_RUNTIME_DEPS_FRAMEWORKS)
            foreach(copy_framework ${${runtime_dep}_RUNTIME_DEPS_FRAMEWORKS})
                add_custom_command(TARGET ${NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${runtime_dep}_BIN_DIR}/${copy_framework} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
                )
            endforeach()
            
        endif()

        # directories
        if(DEFINED ${runtime_dep}_RUNTIME_DEPS_DIRS)
            foreach(copy_dir ${runtime_dep}_RUNTIME_DEPS_DIRS)
                get_filename_component(dir_name ${copy_dir} DIRECTORY)
                add_custom_command(TARGET ${NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${copy_dir} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${dir_name}
                )
            endforeach()
        endif()
        
    endforeach()

endfunction()


