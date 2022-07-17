cmake_minimum_required(VERSION 3.22)

project(Engine CXX)

add_library(${PROJECT_NAME} SHARED
        src/ApplicationBase.cpp src/Rendering/Vulkan/EngineFactoryVk.cpp src/Rendering/Vulkan/EngineContextVk.cpp src/Rendering/Vulkan/DeviceContextVk.cpp src/Rendering/Vulkan/SwapChainVk.cpp src/Rendering/Vulkan/RenderContextVk.cpp src/Rendering/Vulkan/GraphicsPipelineStateVk.cpp src/Rendering/Vulkan/ShaderVk.cpp src/IO/Directories.cpp src/IO/FileIO.cpp src/Rendering/Vulkan/BufferVk.cpp src/IO/FileManager.cpp src/Rendering/Vulkan/TextureVk.cpp src/Utils/Image/stb_lib.cpp src/Rendering/Vulkan/ShaderResourceBindingVk.cpp src/Rendering/Vulkan/TextureViewVk.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include src ${INCLUDE_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_INTERNAL=1)

set(SPIRV_LIBS shaderc glslang MachineIndependent GenericCodeGen OSDependent OGLCompiler SPIRV SPIRV-Tools spirv-cross-cpp spirv-cross-reflect spirv-cross-util spirv-cross-core spirv-cross-glsl)

if (PLATFORM_WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL Debug)
    list(TRANSFORM SPIRV_LIBS APPEND "d")
endif()

if (PLATFORM_WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            vulkan-1.lib SDL2.lib ${SPIRV_LIBS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_DLL_EXPORT=1)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
            vulkan vulkan.1 SDL2 ${SPIRV_LIBS})
endif()

