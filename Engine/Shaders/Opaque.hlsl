

// Helpful for intellisense
#ifndef COMPILE
#define VERTEX 1
#define FRAGMENT 1
#endif

#include "Core/Macros.hlsli"
#include "Core/SceneData.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/ObjectData.hlsli"

#include "Core/PBR/BRDF.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    INSTANCING()
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 bitangent : TEXCOORD4;
};

#if VERTEX
PSInput VertMain(VSInput input)
{
    PSInput output;
    output.position = LOCAL_TO_CLIP_SPACE(float4(input.position, 1.0), input);
    output.worldPos = LOCAL_TO_WORLD_SPACE(float4(input.position, 1.0), input).xyz;
    output.normal = LOCAL_TO_WORLD_SPACE(float4(input.normal, 0), input).xyz;
    output.tangent = LOCAL_TO_WORLD_SPACE(input.tangent, input).xyz;
    output.bitangent = cross(output.normal, output.tangent);
    output.uv = input.uv;
    return output;
}
#endif


#if FRAGMENT

cbuffer _MaterialData : SRG_PerMaterial(b0)
{
    float4 _Albedo;
    float _SpecularStrength;
    float _Metallic;
    float _Roughness;
    uint _Shininess;
};

//Texture2D<float4> _Albedo : SRG_PerMaterial(t1);

// pixel shader function
float4 FragMain(PSInput input) : SV_TARGET
{
    float3 diffuse = 0;
    float3 specular = 0;
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(viewPosition - input.worldPos);

    MaterialInput material;
    material.albedo = _Albedo.rgb;
    material.metallic = _Metallic;
    material.roughness = _Roughness;

    float3 color = float3(0, 0, 0);

    uint i = 0;
    for (i = 0; i < totalDirectionalLights; i++)
    {
        LightInput light;
        light.lightRadiance = _DirectionalLights[i].colorAndIntensity.rgb * _DirectionalLights[i].colorAndIntensity.a;
        light.lightDir = -_DirectionalLights[i].direction;
        light.normal = normal;
        light.viewDir = viewDir;
        light.halfway = normalize(viewDir + light.lightDir);

        color += CalculateBRDF(light, material);

        // float4 lightColor = _DirectionalLights[i].colorAndIntensity;
        // diffuse += max(dot(normal, lightDir), 0) * lightColor.rgb * lightColor.a;

        // float3 reflectDir = reflect(-lightDir, normal);
        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        // specular += _SpecularStrength * spec * lightColor.rgb;
    }

    for (i = 0; i < totalPointLights; i++)
    {
        // float3 lightDir = normalize(_PointLights[i].position - input.worldPos);
        // float4 lightColor = _PointLights[i].colorAndIntensity;
        // diffuse += max(dot(normal, lightDir), 0) * lightColor.rgb * lightColor.a;

        // float3 halfwayDir = normalize(lightDir + viewDir);  
        // float spec = pow(max(dot(normal, halfwayDir), 0.0), _Shininess);
        // specular += _SpecularStrength * spec * lightColor.rgb;
    }
    
    return float4(color, 1.0);
}
#endif
