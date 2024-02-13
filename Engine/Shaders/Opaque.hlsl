

#include "Core/Macros.hlsli"
#include "Core/SceneData.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/ObjectData.hlsli"

#include "Core/Gamma.hlsli"

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
    //output.normal = input.normal;
    output.tangent = LOCAL_TO_WORLD_SPACE(float4(input.tangent.xyz, 0), input).xyz;
    output.bitangent = -cross(output.normal, output.tangent);
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
    float _NormalStrength;
    uint _Shininess;
    float _AmbientOcclusion;
};

Texture2D<float4> _AlbedoTex : SRG_PerMaterial(t1);
Texture2D<float> _RoughnessTex : SRG_PerMaterial(t2);
Texture2D<float4> _NormalMap : SRG_PerMaterial(t3);
Texture2D<float> _MetallicTex : SRG_PerMaterial(t4);

inline float3 GetShadowMapDebug(in float4 lightSpacePos)
{
    float3 projectionCoords = lightSpacePos.xyz / lightSpacePos.w;
    projectionCoords = projectionCoords * float3(0.5, 0.5, 1.0) + float3(0.5, 0.5, 0); // In Vulkan, Z axis is already if [0, 1] range, so we only map X and Y to [0, 1] range FROM [-1, 1] range
    float closestDepth = DirectionalShadowMap.Sample(_ShadowMapSampler, projectionCoords.xy); // This works perfectly fine!
    float currentDepth = projectionCoords.z;
    float bias = 0.005;
    return currentDepth > closestDepth  ? 1.0 : 0.0;
}

// pixel shader function
float4 FragMain(PSInput input) : SV_TARGET
{
    float3 diffuse = 0;
    float3 specular = 0;
    float3 vertNormal = normalize(input.normal);
    float3 viewDir = normalize(viewPosition - input.worldPos);
    float3 tangent = normalize(input.tangent);
    float3 bitangent = normalize(input.bitangent);

    float4 normalMapSample = _NormalMap.Sample(_DefaultSampler, input.uv);
    float3 tangentSpaceNormal = normalize(normalMapSample.xyz * 2.0 - 1.0);

    float3x3 tangentToWorld = float3x3(tangent, bitangent, vertNormal);
    float3 normal = normalize(mul(tangentSpaceNormal, tangentToWorld));

    MaterialInput material;
    material.albedo = GammaToLinear(_Albedo.rgb * _AlbedoTex.Sample(_DefaultSampler, input.uv).rgb);
    //material.albedo = _Albedo.rgb * _AlbedoTex.Sample(_DefaultSampler, input.uv).rgb;
    material.metallic = _Metallic * _MetallicTex.Sample(_DefaultSampler, input.uv);
    material.roughness = _Roughness * _RoughnessTex.Sample(_DefaultSampler, input.uv);

    float3 Lo = float3(0, 0, 0);

    uint i = 0;
    for (i = 0; i < totalDirectionalLights; i++)
    {
        LightInput light;
        light.lightRadiance = _DirectionalLights[i].colorAndIntensity.rgb * _DirectionalLights[i].colorAndIntensity.a;
        light.lightDir = -_DirectionalLights[i].direction;
        light.normal = normal;
        light.viewDir = viewDir;
        light.halfway = normalize(viewDir + light.lightDir);

        float4 lightSpacePos = mul(float4(input.worldPos, 1.0), _DirectionalLights[i].lightSpaceMatrix);
        float shadow = CalculateDirectionalShadow(lightSpacePos, dot(vertNormal, light.lightDir));
        shadow = clamp(shadow, 0, 1);

        Lo += CalculateBRDF(light, material) * (1.0 - shadow);
    }

    for (i = 0; i < totalPointLights; i++)
    {

    }
    
    float3 ambient = float3(0.03, 0.03, 0.03) * material.albedo * _AmbientOcclusion;
    float3 color = Lo;//ambient + Lo;

    color = color / (color + float3(1.0, 1.0, 1.0)); // Tonemapping (optional)
    color = LinearToGamma(color); // Convert to Gamma space
    return float4(color, 1.0);
}
#endif
