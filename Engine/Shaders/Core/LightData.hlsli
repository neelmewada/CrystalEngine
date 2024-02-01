#ifndef __LIGHT_DATA_HLSL__
#define __LIGHT_DATA_HLSL__

#define MAX_DIRECTIONAL_LIGHTS 8

struct DirectionalLight
{
    float3 direction;
    float3 color;
    float intensity;
    float temperature;
};

struct PointLight
{
    float3 position;
    float3 color;
    float intensity;
    float radius;
    float attenuation;
};

cbuffer _DirectionalLightsArray : SRG_PerScene(b)
{
    DirectionalLight _DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
};

//StructuredBuffer<PointLight> _PointLights : SRG_PerScene(t);

cbuffer _LightData : SRG_PerScene(b)
{
    uint totalDirectionalLights;
    //uint maxPointLights;
};

#endif // __LIGHT_DATA_HLSL__