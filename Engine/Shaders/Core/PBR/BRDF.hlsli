#ifndef __BRDF_HLSL__
#define __BRDF_HLSL__

struct MaterialInput
{
    float3 albedo;
    float metallic;
    float roughness;
    float ambient;
};

struct LightInput
{
    float3 lightRadiance;
    float3 normal;
    float3 halfway;
    float3 lightDir;
    float3 viewDir;
};

inline float3 FresnelSchlick(float3 F0, float HdotV)
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

float GeometrySchlickGGX(float dotProduct, float roughness)
{
    float r = (roughness + 1.0);
    float a = roughness * roughness;
    float k = (r*r) / 8.0;
    //float k = a / 2;

    float num   = dotProduct;
    float denom = dotProduct * (1.0 - k) + k;
	denom = max(denom, 0.0001);

    return num / denom;
}

inline float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float FresnelSchlick90(float cosTheta, float F0, float F90) {
  return F0 + (F90 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

float DisneyDiffuseFactor(float NdotV, float NdotL, float VdotH, float roughness) {
  float alpha = roughness * roughness;
  float F90 = 0.5 + 2.0 * alpha * VdotH * VdotH;
  float F_in = FresnelSchlick90(NdotL, 1.0, F90);
  float F_out = FresnelSchlick90(NdotV, 1.0, F90);
  return F_in * F_out;
}

float3 CalculateBRDF(LightInput light, MaterialInput material)
{
    float NdotH = clamp(dot(light.normal, light.halfway), 0.0, 1.0);
    float NdotL = clamp(dot(light.normal, light.lightDir), 0.0, 1.0);
    float NdotV = clamp(dot(light.normal, light.viewDir), 0.0, 1.0);
    float HdotV = clamp(dot(light.halfway, light.viewDir), 0.0, 1.0);

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, material.albedo.rgb, material.metallic);
    float3 F = FresnelSchlick(F0, HdotV);

    // Specular
    float NDF = DistributionGGX(NdotH, material.roughness);
    float G = GeometrySmith(NdotV, NdotL, material.roughness);

    float3 numerator    = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    denominator = max(denominator, 0.0001);
    float3 specular     = numerator / denominator; 
    float3 kS = F;

    // Diffuse
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= (1 - material.metallic);

    // Remove below line for better performance
    //kD *= DisneyDiffuseFactor(NdotV, NdotL, HdotV, material.roughness);

    float3 diffuse = kD * material.albedo.rgb / PI;

    return (diffuse + specular) * light.lightRadiance * NdotL;
}

#endif
