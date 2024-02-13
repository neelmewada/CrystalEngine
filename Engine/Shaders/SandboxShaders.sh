#!/bin/sh

exe=""

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    platform="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    platform="mac"
elif [[ "$OSTYPE" == "cygwin" ]]; then
    platform="windows"
elif [[ "$OSTYPE" == "msys" ]]; then
    platform="windows"
elif [[ "$OSTYPE" == "win32" ]]; then
    platform="windows"
    exe=".exe"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
    platform="linux"
else
    echo "Unknown Operating System"
    exit 1
fi

DxCompiler=${DXC}

CompileShader() {

    local FileName=$1".hlsl"
    local VertOut="../Source/VulkanSandbox/Resources/Shaders/"$1".vert.spv"
    local FragOut="../Source/VulkanSandbox/Resources/Shaders/"$1".frag.spv"

    ${DxCompiler} -spirv -E VertMain -T vs_6_0 -D PLATFORM_DESKTOP=1 -D COMPILE=1 -D VERTEX=1 -fspv-preserve-bindings -fspv-debug=vulkan-with-source -Fo ${VertOut} ${FileName} #-fspv-debug=vulkan-with-source #-fvk-use-dx-layout
    ${DxCompiler} -spirv -E FragMain -T ps_6_0 -D PLATFORM_DESKTOP=1 -D COMPILE=1 -D FRAGMENT=1 -fspv-preserve-bindings -fspv-debug=vulkan-with-source -Fo ${FragOut} ${FileName} #-fspv-debug=vulkan-with-source #-fvk-use-dx-layout

    echo "Compiled: " ${FileName}
}

CompileCS() {
    local FileName=$1".hlsl"
    local Out="../Source/VulkanSandbox/Resources/Shaders/"$1".spv"

    ${DxCompiler} -spirv -E CSMain -T cs_6_0 -D PLATFORM_DESKTOP=1 -fspv-preserve-bindings -fspv-extension=KHR -Fo ${Out} ${FileName}

    echo "Compiled: " ${FileName}
}

CompileShader "Depth"
CompileShader "Opaque"
CompileShader "Skybox"
CompileShader "Equirectangular"
CompileShader "EnvMapConvolution"

