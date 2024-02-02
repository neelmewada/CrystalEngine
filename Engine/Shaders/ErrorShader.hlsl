
#include "ShaderLibrary.hlsl"

// data structure : before vertex shader (mesh info)
struct VertexInfo
{
    float3 position : POSITION;
};

struct v2f
{
    float4 position : SV_POSITION;
};

// vertex shader function
v2f VertMain(VertexInfo input)
{
    v2f output;
    output.position = mul(mul(mul(float4(input.position, 1.0), modelMatrix), viewMatrix), projectionMatrix);
    return output;
}

// pixel shader function
float4 FragMain(v2f input) : SV_TARGET
{
    return float4(1.0, 0, 1.0, 1.0);
}

