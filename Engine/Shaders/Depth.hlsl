
//#define USE_INSTANCING

#include "Core/Macros.hlsli"
#include "Core/SceneData.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/ObjectData.hlsli"

struct VSInput
{
    float3 position : POSITION;
    INSTANCING()
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VertMain(VSInput input)
{
    VSOutput output;
    output.position = LOCAL_TO_CLIP_SPACE(float4(input.position, 1.0), input);
    return output;
}

// pixel shader function
float4 FragMain(VSOutput input) : SV_TARGET
{
    return float4(1.0, 1.0, 1.0, 1.0);
}


