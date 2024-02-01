
//#define USE_INSTANCING

#include "Core/Macros.hlsli"
#include "Core/SceneData.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/ObjectData.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    INSTANCING()
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

cbuffer _MaterialData : SRG_PerMaterial(b)
{
    float4 albedo;
};

PSInput VertMain(VSInput input)
{
    PSInput output;
    output.position = LOCAL_TO_CLIP_SPACE(float4(input.position, 1.0), input);
    output.worldPos = LOCAL_TO_WORLD_SPACE(float4(input.position, 1.0), input).xyz;
    output.normal = LOCAL_TO_WORLD_SPACE(float4(input.normal, 0), input).xyz;
    return output;
}

float Map01(in float value)
{
    return (value + 1) / 2.0;
}

// pixel shader function
float4 FragMain(PSInput input) : SV_TARGET
{
    float3 diffuse = 0;
    float3 normal = normalize(input.normal);
    for (uint i = 0; i < totalDirectionalLights; i++)
    {
        diffuse += max(dot(normal, _DirectionalLights[i].direction), 0) * _DirectionalLights[i].color;
    }
    return float4(Map01(normal.x), 0, 0, 1.0);
    //return float4(1.0, 0, 1.0, 1.0);
}
