
#include "Core/Macros.hlsli"

#include "Core/Macros.hlsli"
#include "Core/Gamma.hlsli"


struct VSInput
{
    float3 position : POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 localPosition : TEXCOORD0;
};


cbuffer _PerViewData : SRG_PerPass(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
    float3 viewPosition;
};

#if VERTEX
PSInput VertMain(VSInput input)
{
    PSInput o;
    o.localPosition = input.position;
    o.position = mul(float4(input.position, 1.0), viewProjectionMatrix);
    return o;
}
#endif


#if FRAGMENT

TextureCube<float4> _CubeMap : SRG_PerPass(t1);
SamplerState _CubeMapSampler : SRG_PerPass(t2);

float4 FragMain(PSInput input) : SV_Target
{
    float3 normal = normalize(input.localPosition);
    float3 irradiance = float3(0, 0, 0);

    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0; 

    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += _CubeMap.Sample(_CubeMapSampler, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    return float4(irradiance, 1);
}

#endif


