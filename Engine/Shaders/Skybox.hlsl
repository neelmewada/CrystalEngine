
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
    float3 worldPos : TEXCOORD0;
};

PSInput VertMain(VSInput input)
{
    PSInput o;
    o.worldPos = LOCAL_TO_WORLD_SPACE(float4(input.position, 1.0), input).xyz;
    o.worldPos += viewPosition;
    o.position = WORLD_TO_CLIP_SPACE(float4(o.worldPos, 1.0));
    return o;
}

float4 FragMain(PSInput input) : SV_TARGET
{
    float3 direction = normalize(input.worldPos - viewPosition);
    return float4(_Skybox.Sample(_DefaultSampler, direction).rgb, 1);
}

