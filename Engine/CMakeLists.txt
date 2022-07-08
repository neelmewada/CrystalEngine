cmake_minimum_required(VERSION 3.22)

project(Engine CXX)

add_library(${PROJECT_NAME} SHARED
        src/ApplicationBase.cpp src/Rendering/Vulkan/EngineFactoryVk.cpp src/Rendering/Vulkan/EngineContextVk.cpp src/Rendering/Vulkan/DeviceContextVk.cpp src/Rendering/Vulkan/SwapChainVk.cpp src/Rendering/Vulkan/RenderContextVk.cpp src/Rendering/Vulkan/GraphicsPipelineStateVk.cpp src/Rendering/Vulkan/ShaderVk.cpp src/IO/Directories.cpp src/IO/FileIO.cpp src/Rendering/Vulkan/BufferVk.cpp src/IO/FileManager.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include src ${INCLUDE_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})

if (PLATFORM_WIN32)
    target_link_libraries(${PROJECT_NAME}
            vulkan-1 SDL2)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_DLL_EXPORT=1)
else()
    target_link_libraries(${PROJECT_NAME}
            vulkan vulkan.1 SDL2 spirv-cross-reflect)
endif()

