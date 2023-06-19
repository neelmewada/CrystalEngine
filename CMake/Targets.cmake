
include_guard(GLOBAL)

ce_set(TARGET_TYPE_SHARED_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_STATIC_IS_LIBRARY TRUE)
#ce_set(TARGET_TYPE_INTERFACE_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_MODULE_IS_LIBRARY TRUE)
ce_set(TARGET_TYPE_CONSOLEAPP_IS_EXECUTABLE TRUE)
ce_set(TARGET_TYPE_GUIAPP_IS_EXECUTABLE TRUE)
ce_set(TARGET_TYPE_TOOL_IS_EXECUTABLE TRUE)

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

# \arg:TARGET_TYPE: SHARED; STATIC; MODULE; CONSOLEAPP ; GUIAPP ; TOOL
function(ce_add_target NAME TARGET_TYPE)
    string(TOUPPER ${NAME} NAME_UPPERCASE)

    set(options AUTORTTI AUTOMOC AUTOUIC AUTORCC COPY_CONFIGS VS_STARTUP_PROJECT)
    set(oneValueArgs VERSION OUTPUT_SUBDIRECTORY FOLDER NAMESPACE OUTPUT_DIRECTORY PACKAGE)
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

        if(${ce_add_target_VS_STARTUP_PROJECT})
            set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NAME})
        endif()
        
        
    else()
        message(FATAL_ERROR "Invalid TARGET_TYPE passed: ${TARGET_TYPE}")
    endif()
    
    ce_group_sources_by_folder(${NAME})

    # Per Source Definitions

    # foreach(source_file ${SOURCES})
    #     cmake_path(GET source_file FILENAME source_filename)
    #     cmake_path(REPLACE_EXTENSION source_filename LAST_ONLY ".h")
    #     string(REPLACE "." "_" source_filename_modified "${source_filename}")
    #     string(PREPEND source_filename_modified "${NAME}_")
        
    #     #set_source_files_properties(${source_file} PROPERTIES COMPILE_DEFINITIONS -D__FILE_ID__=${source_filename_modified})
    # endforeach()

    # PACKAGE

    target_compile_definitions(${NAME} PRIVATE PACKAGE_NAME="/Code/${NAME}")
    

    # OUTPUT_DIRECTORY

    if(${ce_add_target_OUTPUT_DIRECTORY})
        set_target_properties(${NAME}
            PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
                LIBRARY_OUTPUT_DIRECTORY "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
        )
    endif()

    # COPY_CONFIGS

    if(ce_add_target_COPY_CONFIGS)
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../Config ${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}/Config
        )
    endif()

    # FOLDER
    
    if(ce_add_target_FOLDER)
        set_target_properties(${NAME} 
            PROPERTIES
                FOLDER "${ce_add_target_FOLDER}"
        )
    endif()

    # INCLUDE_DIRECTORIES

    set(multiValueArgs PRIVATE PUBLIC INTERFACE)
    cmake_parse_arguments(ce_add_target_INCLUDE_DIRECTORIES "" "" "${multiValueArgs}" ${ce_add_target_INCLUDE_DIRECTORIES})

    if(${ce_add_target_AUTORTTI})
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Generated")
        list(APPEND ce_add_target_INCLUDE_DIRECTORIES_PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/Generated")
    endif()

    target_include_directories(${NAME}
        PRIVATE   ${ce_add_target_INCLUDE_DIRECTORIES_PRIVATE}
        PUBLIC    ${ce_add_target_INCLUDE_DIRECTORIES_PUBLIC}
        INTERFACE ${ce_add_target_INCLUDE_DIRECTORIES_INTERFACE}
    )
    
    # BUILD_DEPENDENCIES

    set(multiValueArgs PRIVATE PUBLIC INTERFACE TARGETS MACFRAMEWORKS)
    cmake_parse_arguments(ce_add_target_BUILD_DEPENDENCIES "" "" "${multiValueArgs}" ${ce_add_target_BUILD_DEPENDENCIES})

    if(${PAL_PLATFORM_IS_MAC})
        list(APPEND ce_add_target_BUILD_DEPENDENCIES_PRIVATE "c")
        list(APPEND ce_add_target_BUILD_DEPENDENCIES_PRIVATE "c++")
        list(APPEND ce_add_target_BUILD_DEPENDENCIES_PRIVATE "-framework CoreServices")
        if(ce_add_target_BUILD_DEPENDENCIES_MACFRAMEWORKS)
            foreach(framework ${ce_add_target_BUILD_DEPENDENCIES_MACFRAMEWORKS})
                list(APPEND ce_add_target_BUILD_DEPENDENCIES_PRIVATE "-framework ${framework}")
            endforeach()
        endif()
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

    # PCH

    if(ce_add_target_PCHHEADER)
        set(oneValueArgs REUSE_FROM)
        set(multiValueArgs PRIVATE PUBLIC INTERFACE)
        cmake_parse_arguments(ce_add_target_PCHHEADER "" "${oneValueArgs}" "${multiValueArgs}" ${ce_add_target_PCHHEADER})

        if(ce_add_target_PCHHEADER_REUSE_FROM)
            target_precompile_headers(${NAME} REUSE_FROM ${ce_add_target_PCHHEADER_REUSE_FROM})
        else()
            target_precompile_headers(${NAME}
                PUBLIC    ${ce_add_target_PCHHEADER_PUBLIC}
                PRIVATE   ${ce_add_target_PCHHEADER_PRIVATE}
                INTERFACE ${ce_add_target_PCHHEADER_INTERFACE}
            )
        endif()
    endif()

    # COMPILE_DEFINITIONS

    set(multiValueArgs PRIVATE PUBLIC INTERFACE)
    cmake_parse_arguments(ce_add_target_COMPILE_DEFINITIONS "" "" "${multiValueArgs}" ${ce_add_target_COMPILE_DEFINITIONS})
    
    if((${PAL_PLATFORM_IS_WINDOWS}) EQUAL 1 AND (${TARGET_TYPE_${TARGET_TYPE}_IS_SHAREDLIB}) EQUAL 1)
        list(APPEND ce_add_target_COMPILE_DEFINITIONS_PRIVATE "${NAME_UPPERCASE}_API=__declspec(dllexport)")
        list(APPEND ce_add_target_COMPILE_DEFINITIONS_INTERFACE  "${NAME_UPPERCASE}_API=__declspec(dllimport)")
    else()
        list(APPEND ce_add_target_COMPILE_DEFINITIONS_PUBLIC "${NAME_UPPERCASE}_API=")
    endif()

    target_compile_definitions(${NAME} 
        PRIVATE   ${ce_add_target_COMPILE_DEFINITIONS_PRIVATE} 
        PUBLIC    ${ce_add_target_COMPILE_DEFINITIONS_PUBLIC}
        INTERFACE ${ce_add_target_COMPILE_DEFINITIONS_INTERFACE}
    )
    
    # AUTORTTI

    if(${ce_add_target_AUTORTTI})
        target_compile_definitions(${NAME} PRIVATE _AUTORTTI=1)

        set(AutoRttiCmd "AutoRTTI")

        add_custom_command(TARGET ${NAME} PRE_BUILD
            COMMAND "AutoRTTI" -m ${NAME} -d "${CMAKE_CURRENT_SOURCE_DIR}/" -o "${CMAKE_CURRENT_BINARY_DIR}/Generated"
            VERBATIM
        )

    else()
        target_compile_definitions(${NAME} PRIVATE _AUTORTTI=0)
    endif()
    
    # RUNTIME_DEPENDENCIES

    foreach(runtime_dep ${ce_add_target_RUNTIME_DEPENDENCIES})
        # NEW Loading
        if(TARGET ${runtime_dep}_RT)
            set(target_runtime_deps "")
            get_target_property(root_path ${runtime_dep}_RT ROOT_PATH)
            get_target_property(copy_dirs ${runtime_dep}_RT COPY_DIRS)
            get_target_property(copy_libs ${runtime_dep}_RT COPY_LIBS)
            get_target_property(copy_files ${runtime_dep}_RT COPY_FILES)

            if(copy_libs)
                foreach(copy_lib ${copy_libs})
                    ce_decorated_lib_name(${copy_lib} copy_lib)
                    if(NOT (IS_ABSOLUTE ${copy_lib}))
                        set(copy_lib "${root_path}/${copy_lib}")
                    endif()
                    
                    add_custom_command(TARGET ${NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${copy_lib} "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
                    )
                    list(APPEND target_runtime_deps "${copy_lib}")
                endforeach()
            endif()

            set_target_properties(${NAME} 
                PROPERTIES
                    DLL_DEPS "${target_runtime_deps}"
            )
            set(target_runtime_deps "")

            if(copy_files)
                foreach(copy_file ${copy_files})
                    if(NOT (IS_ABSOLUTE ${copy_file}))
                        set(copy_file "${root_path}/${copy_file}")
                    endif()
                    
                    add_custom_command(TARGET ${NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${copy_file} "${CE_OUTPUT_DIR}/${ce_add_target_OUTPUT_DIRECTORY}"
                    )
                    list(APPEND target_runtime_deps "${copy_file}")
                endforeach()
            endif()
            
            continue()
        endif()
        

        # OLD Loading: Dynamic libraries
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
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${${runtime_dep}_LIB_DIR}/${copy_framework} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${copy_framework}
                )
            endforeach()
        endif()

        # directories
        if(DEFINED ${runtime_dep}_RUNTIME_DEPS_DIRS)
            foreach(copy_dir ${${runtime_dep}_RUNTIME_DEPS_DIRS})
                get_filename_component(dir_name ${copy_dir} NAME)
                add_custom_command(TARGET ${NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${copy_dir} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${dir_name}
                )
            endforeach()
        endif()
        
    endforeach()

endfunction()



