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

DxCompiler="../../ThirdParty/dxcompiler-1.7.2212-rev1-${platform}/dxcompiler/Release/dxc${exe}"

CompileShader() {

    local FileName=$1".hlsl"
    local VertOut="../Source/VulkanSandbox/Resources/Shaders/"$1".vert.spv"
    local FragOut="../Source/VulkanSandbox/Resources/Shaders/"$1".frag.spv"

    ${DxCompiler} -spirv -E VertMain -T vs_6_0 -fspv-debug=vulkan-with-source -Fo ${VertOut} ${FileName} #-fvk-use-dx-layout
    ${DxCompiler} -spirv -E FragMain -T ps_6_0 -fspv-debug=vulkan-with-source -Fo ${FragOut} ${FileName} #-fvk-use-dx-layout

    echo "Compiled:" ${FileName}
}

CompileShader "Depth"
CompileShader "Opaque"

