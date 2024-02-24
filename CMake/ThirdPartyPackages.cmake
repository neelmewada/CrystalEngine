include_guard(GLOBAL)

# Utils
find_package(xxHash REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml REQUIRED)
find_package(mINI REQUIRED)
find_package(cxxopts REQUIRED)
find_package(efsw REQUIRED)
find_package(zip REQUIRED)
find_package(crcpp REQUIRED)
find_package(stb REQUIRED)
find_package(dxcompiler REQUIRED)
#find_package(compressonator REQUIRED)
find_package(miniz REQUIRED)
find_package(ispctexturecompressor REQUIRED)

# Yoga
find_package(yoga REQUIRED)

# SDL2
find_package(sdl REQUIRED)

# Vulkan
if(${PAL_TRAIT_VULKAN_SUPPORTED})
    find_package(Vulkan REQUIRED
        COMPONENTS SPIRV-Tools
    )

    set(Vulkan_RUNTIME_DEPS "")

    if(${PAL_PLATFORM_IS_WINDOWS})
        set(Vulkan_BIN_DIR "$ENV{VULKAN_SDK}/Bin")
        list(APPEND Vulkan_RUNTIME_DEPS "VkLayer_khronos_validation.dll")
    elseif(${PAL_PLATFORM_IS_MAC})
        set(Vulkan_BIN_DIR "$ENV{VULKAN_SDK}/macOS/lib")
        set(Vulkan_LIB_DIR "$ENV{VULKAN_SDK}/macOS/lib")
    endif()

    link_directories(AFTER $ENV{VULKAN_SDK}/lib)

    # Vulkan SpirV Tools
    add_library(SpirvTools ALIAS Vulkan::SPIRV-Tools)

    target_link_libraries(Vulkan::SPIRV-Tools
        INTERFACE 
            "SPIRV" 
            "SPIRV-Tools" 
            "spirv-cross-reflect$<${PAL_PLATFORM_IS_WINDOWS}:$<$<CONFIG:Debug>:d>>" 
            "spirv-cross-core$<${PAL_PLATFORM_IS_WINDOWS}:$<$<CONFIG:Debug>:d>>" 
            "spirv-cross-cpp$<${PAL_PLATFORM_IS_WINDOWS}:$<$<CONFIG:Debug>:d>>"
            "spirv-cross-glsl$<${PAL_PLATFORM_IS_WINDOWS}:$<$<CONFIG:Debug>:d>>"
            "spirv-cross-hlsl$<${PAL_PLATFORM_IS_WINDOWS}:$<$<CONFIG:Debug>:d>>"
    )

    ce_add_rt_deps(SpirvTools
        ROOT_PATH "$ENV{VULKAN_SDK}/Bin"
        MAC_ROOT_PATH "$ENV{VULKAN_SDK}/lib"
        COPY_LIBS
            $<${PAL_PLATFORM_IS_WINDOWS}:SPIRV-Tools-shared$<$<CONFIG:Debug>:d>.dll>
            $<${PAL_PLATFORM_IS_MAC}:libSPIRV-Tools-shared.dylib>
    )
    
endif()

# Metal
if(${PAL_TRAIT_METAL_SUPPORTED})
    find_package(metalcpp)
endif()

