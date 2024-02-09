#ifndef __BRDF_HLSL__
#define __BRDF_HLSL__

#define PI 3.14159265359

struct MaterialInput
{
    float3 albedo;
    float metallic;
    float roughness;
};

struct LightInput
{
    float3 lightRadiance;
    float3 normal;
    float3 halfway;
    float3 lightDir;
    float3 viewDir;
};

inline float3 FresnelSchlick(float3 F0, float3 HdotV)
{
    return F0 + (1 - F0) * pow(clamp(1 - HdotV, 0, 1), 5);
}

float DistributionGGX(float NdotH, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH2 = NdotH * NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

inline float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float3 CalculateBRDF(LightInput light, MaterialInput material)
{
    // Just for experimenting with roughness value
    material.roughness = material.roughness * material.roughness;

    float NdotH = max(dot(light.normal, light.halfway), 0.0);
    float NdotL = max(dot(light.normal, light.lightDir), 0.0);
    float NdotV = max(dot(light.normal, light.viewDir), 0.0);
    float HdotV = max(dot(light.halfway, light.viewDir), 0.0);

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, material.albedo.rgb, material.metallic);
    float3 F = FresnelSchlick(F0, HdotV);

    // Specular
    float NDF = DistributionGGX(NdotH, material.roughness);
    float G = GeometrySmith(NdotV, NdotL, material.roughness);

    float3 numerator    = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL  + 0.0001;
    float3 specular     = numerator / denominator; 
    float3 kS = F;

    // Diffuse
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= (1 - material.metallic);

    float3 lambert = kD * material.albedo.rgb / PI;

    return (lambert + specular) * light.lightRadiance * NdotL;
}

#endif
