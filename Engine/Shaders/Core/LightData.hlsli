#ifndef __LIGHT_DATA_HLSL__
#define __LIGHT_DATA_HLSL__

#define MAX_DIRECTIONAL_LIGHTS 8

struct DirectionalLight
{
    float3 direction;
    float4 colorAndIntensity;
    float temperature;
};

struct PointLight
{
    float3 position;
    float4 colorAndIntensity;
    float radius;
    float attenuation;
};

cbuffer _DirectionalLightsArray : SRG_PerScene(b)
{
    DirectionalLight _DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
};

StructuredBuffer<PointLight> _PointLights : SRG_PerScene(t);

cbuffer _LightData : SRG_PerScene(b)
{
    float4 ambient;
    uint totalDirectionalLights;
    uint totalPointLights;
};

#endif // __LIGHT_DATA_HLSL__