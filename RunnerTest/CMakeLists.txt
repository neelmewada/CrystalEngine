cmake_minimum_required(VERSION 3.22)

project(RunnerTest CXX)

# This little macro lets you set any XCode specific property
macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

if (PLATFORM_MACOS)
    set(EXEC_TYPE MACOSX_BUNDLE)
    if (NOT GENERATOR_XCODE)
        set(CMAKE_SKIP_BUILD_RPATH TRUE)
    endif()

    set(APP_BUNDLE_CONTENTS_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents")
    set(APP_BUNDLE_FRAMEWORKS_DIR "${APP_BUNDLE_CONTENTS_DIR}/Frameworks")
endif()

# Retrieve all pShader source files
file(GLOB_RECURSE VERT_SHADER_SRCS shaders/*.vert)
file(GLOB_RECURSE FRAG_SHADER_SRCS shaders/*.frag)
file(GLOB_RECURSE HLSL_SRCS        shaders/*.hlsl)

include(Shaders.cmake)
# Compile Shaders during CMake Configuration
compile_vert_shaders("${VERT_SHADER_SRCS}")
compile_frag_shaders("${FRAG_SHADER_SRCS}")
compile_hlsl_shaders("${HLSL_SRCS}")
file(COPY shaders DESTINATION      ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB_RECURSE VERT_SHADER_SRCS ${CMAKE_CURRENT_BINARY_DIR}/shaders/*.vert)
file(GLOB_RECURSE FRAG_SHADER_SRCS ${CMAKE_CURRENT_BINARY_DIR}/shaders/*.frag)
file(GLOB_RECURSE HLSL_SRCS        ${CMAKE_CURRENT_BINARY_DIR}/shaders/*.hlsl)
file(REMOVE "${VERT_SHADER_SRCS}" "${FRAG_SHADER_SRCS}" "${HLSL_SRCS}")

add_executable(${PROJECT_NAME}
        ${EXEC_TYPE}
        src/main.cpp)

add_dependencies(${PROJECT_NAME} Engine)

if (PLATFORM_MACOS)
    SET_XCODE_PROPERTY(${PROJECT_NAME} CODE_SIGN_IDENTITY ${APPLE_DEV_CERT_NAME})#"Apple Developer")
    SET_XCODE_PROPERTY(${PROJECT_NAME} DEVELOPMENT_TEAM ${APPLE_DEV_TEAM_ID})

    # Copy the resources to the macOS app bundle
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources/shaders)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
        ../Engine/include ${INCLUDE_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Engine vulkan-1 SDL2)
elseif (APPLE) # macOS
    set(LIBS vulkan vulkan.1 SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Engine ${LIBS})
endif()

if (PLATFORM_MACOS)
    # CreateGraphicsPipeline Frameworks directory
    execute_process(COMMAND rm -rf ${APP_BUNDLE_FRAMEWORKS_DIR})
    execute_process(COMMAND mkdir ${APP_BUNDLE_FRAMEWORKS_DIR})
    # Copy all the linked libraries to Frameworks directory
    foreach(LIB ${LIBS})
        find_library(LIB_PATH ${LIB})
        message(Copied: " ${LIB_PATH}")
        execute_process(COMMAND cp ${LIB_PATH} ${APP_BUNDLE_FRAMEWORKS_DIR}/)
        unset(LIB_PATH CACHE)
    endforeach()

    if (NOT GENERATOR_XCODE) # Don't modify rpath if generating xcode project
        # Set rpath of the macos executable after build
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND install_name_tool -add_rpath "@executable_path/../Frameworks" "${APP_BUNDLE_CONTENTS_DIR}/MacOS/${PROJECT_NAME}"
                COMMAND install_name_tool -add_rpath ${CMAKE_CURRENT_BINARY_DIR}/../Engine "${APP_BUNDLE_CONTENTS_DIR}/MacOS/${PROJECT_NAME}")

        # Copy the libEngine.dylib on install time
        install(CODE "
                file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../Engine/libEngine.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Frameworks/)
                ")
    endif()
elseif(PLATFORM_WIN32)
    add_custom_target(COPY_DLL
            COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_BINARY_DIR}/Engine.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Engine.dll ${CMAKE_CURRENT_BINARY_DIR}/)
endif()
