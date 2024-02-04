
#include "Core/Macros.hlsli"
#include "Core/SceneData.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/ObjectData.hlsli"

struct VSInput
{
    float3 position : POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VertMain(VSInput input)
{
    PSInput o;
    o.position = LOCAL_TO_CLIP_SPACE(float4(input.position, 1.0), input);
    return o;
}

float4 FragMain(PSInput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}

