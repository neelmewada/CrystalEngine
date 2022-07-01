cmake_minimum_required(VERSION 3.22)

project(Engine)

add_library(${PROJECT_NAME} SHARED
        src/ApplicationBase.cpp src/Rendering/EngineFactoryVk.cpp src/Rendering/EngineContextVk.cpp src/Rendering/DeviceContextVk.cpp src/Rendering/SwapChainVk.cpp src/Rendering/RenderContextVk.cpp src/Rendering/GraphicsPipelineStateVk.cpp src/Rendering/ShaderVk.cpp src/IO/Directories.cpp src/IO/FileIO.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include src ${INCLUDE_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})

if (WIN32)
    target_link_libraries(${PROJECT_NAME}
            vulkan-1 SDL2)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_DLL_EXPORT=1)
else()
    target_link_libraries(${PROJECT_NAME}
            vulkan vulkan.1 SDL2)
endif()
