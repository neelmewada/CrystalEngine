include_guard(GLOBAL)

find_package(Git REQUIRED)

set(BUILD_SHARED_LIBS OFF)
option(BUILD_SHARED_LIBS OFF)

# 3rd Party Packages
#find_package(xxHash REQUIRED)
#find_package(spdlog REQUIRED)
#find_package(efsw REQUIRED)
#find_package(zip REQUIRED)
#find_package(crcpp REQUIRED)
#find_package(stb REQUIRED)
#find_package(miniz REQUIRED)
#find_package(freetype REQUIRED)
#find_package(assimp REQUIRED)

# Enable these
#find_package(dxcompiler REQUIRED)
#find_package(ispctexturecompressor REQUIRED)

FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(tracy)

# SDL2
#find_package(sdl REQUIRED)

# Vulkan
if(${PAL_TRAIT_VULKAN_SUPPORTED})
    find_package(Vulkan REQUIRED
        COMPONENTS SPIRV-Tools dxc
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
    add_library(ThirdParty::SpirvTools ALIAS Vulkan::SPIRV-Tools)
    add_library(ThirdParty::dxcompiler ALIAS Vulkan::dxc_lib)

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

    get_target_property(dxc_lib_path Vulkan::dxc_lib IMPORTED_LOCATION_RELEASE)
    get_filename_component(dxc_lib_dir "${dxc_lib_path}" DIRECTORY)

    ce_add_rt_deps(SpirvTools
        ROOT_PATH "$ENV{VULKAN_SDK}/Bin"
        MAC_ROOT_PATH "$ENV{VULKAN_SDK}/lib"
        COPY_LIBS
            $<${PAL_PLATFORM_IS_WINDOWS}:SPIRV-Tools-shared$<$<CONFIG:Debug>:d>.dll>
            $<${PAL_PLATFORM_IS_MAC}:libSPIRV-Tools-shared.dylib>
            #$<${PAL_PLATFORM_IS_WINDOWS}:dxcompiler$<$<CONFIG:Debug>:d>.dll>
    )

    ce_add_rt_deps(dxcompiler
        ROOT_PATH "$ENV{VULKAN_SDK}/Bin"
        MAC_ROOT_PATH "$ENV{VULKAN_SDK}/lib"
        COPY_LIBS
            $<${PAL_PLATFORM_IS_WINDOWS}:dxcompilerd.dll>
            $<${PAL_PLATFORM_IS_WINDOWS}:dxcompilerd.pdb>
            $<${PAL_PLATFORM_IS_WINDOWS}:dxcompiler.dll>
            $<${PAL_PLATFORM_IS_WINDOWS}:dxcompiler.pdb>
    )
    
endif()

# Metal
if(${PAL_TRAIT_METAL_SUPPORTED})
    find_package(metalcpp)
endif()
