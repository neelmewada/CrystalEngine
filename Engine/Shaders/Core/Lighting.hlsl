#ifndef __LIGHTING_HLSL__
#define __LIGHTING_HLSL__

#define MAX_DIRECTIONAL_LIGHTS 8

#include "PBR/BRDF.hlsli"

#if FRAGMENT

struct DirectionalLight
{
    float4x4 lightSpaceMatrix;
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

cbuffer _DirectionalLightsArray : SRG_PerScene(b0)
{
    DirectionalLight _DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
};

StructuredBuffer<PointLight> _PointLights : SRG_PerScene(t1);

cbuffer _LightData : SRG_PerScene(b2)
{
    float4 ambient;
    uint totalDirectionalLights;
    uint totalPointLights;
};

SamplerState _ShadowMapSampler : SRG_PerScene(s3);

Texture2D<float> DirectionalShadowMap : SRG_PerPass(t0);

float CalculateDirectionalShadow(in float4 lightSpacePos, in float NdotL)
{
    float3 projectionCoords = lightSpacePos.xyz / lightSpacePos.w;
    projectionCoords = projectionCoords * float3(0.5, 0.5, 1.0) + float3(0.5, 0.5, 0); // In Vulkan, Z axis is already if [0, 1] range, so we only map X and Y to [0, 1] range FROM [-1, 1] range
    projectionCoords.z = clamp(projectionCoords.z, 0, 1);
    //float closestDepth = DirectionalShadowMap.Sample(_ShadowMapSampler, projectionCoords.xy); // This works perfectly fine!
    float currentDepth = projectionCoords.z;
    float bias = 0.005;
    bias = max(0.01 * (1.0 - NdotL), 0.005);
    //return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
    float w; float h;
    DirectionalShadowMap.GetDimensions(w, h);
    float2 texelSize = float2(1, 1) / float2(w, h);
    float shadow = 0;
    for (int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float pcfDepth = DirectionalShadowMap.Sample(_ShadowMapSampler, projectionCoords.xy + float2(x, y) * texelSize);
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

#endif // FRAGMENT

#endif // __LIGHTING_HLSL__
