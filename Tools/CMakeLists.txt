cmake_minimum_required(VERSION 3.20)

project(Tools)

add_subdirectory(AutoRTTI)
add_subdirectory(ResourceCompiler)
add_subdirectory(AssetProcessor)

if(${PAL_TRAIT_BUILD_EDITOR})
    add_custom_target(HostTools)

    set_target_properties(HostTools
        PROPERTIES
            FOLDER "Tools"
    )

    add_dependencies(HostTools AutoRTTI ResourceCompiler AssetProcessor EditorAssets)
    
endif()
