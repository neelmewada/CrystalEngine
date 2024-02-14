
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

// Mirror binary digits about the decimal point
float radicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Randomish sequence that has pretty evenly spaced points
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float2 Hammersley(uint i, uint N)
{
    return float2(float(i)/float(N), radicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;

    float Phi = 2 * PI * Xi.x;
    float cosTheta = sqrt( (1 - Xi.y) / ( 1 + (a * a - 1) * Xi.y ) ); 
    float sinTheta = sqrt( 1 - cosTheta * cosTheta );

    float3 H;
    H.x = sinTheta * cos(Phi);
    H.y = sinTheta * sin(Phi);
    H.z = cosTheta;

    float3 upVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangentX = normalize(cross(upVector, N));
    float3 tangentY = cross(N, tangentX);
    return tangentX * H.x + tangentY * H.y + N * H.z;;
}

TextureCube<float4> _CubeMap : SRG_PerPass(t1);
SamplerState _CubeMapSampler : SRG_PerPass(t2);

float4 FragMain(PSInput input) : SV_Target
{
    float3 normal = normalize(input.localPosition);
    float3 irradiance = float3(0, 0, 0);
    float3 N = normal;

    //return float4(_CubeMap.Sample(_CubeMapSampler, normal).rgb, 1.0);

    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    const uint sampleCount = 4096;

    float totalWeight = 0.0f;

    for (uint i = 0; i < sampleCount; i++)
    {
        float2 Xi = Hammersley(i, sampleCount);
        float3 H = ImportanceSampleGGX(Xi, N, 1.0);

        float NdotH = max(dot(N, H), 0.0);
        float D = 1.0 / PI;

        float pdf = (D * NdotH / (4.0)) + 0.0001;
        const float resolution = 1024;

        irradiance += _CubeMap.Sample(_CubeMapSampler, H).rgb * NdotH;
        totalWeight += NdotH;
    }

    irradiance = (PI * irradiance) / totalWeight;

    // for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    // {
    //     for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
    //     {
    //         // spherical to cartesian (in tangent space)
    //         float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
    //         // tangent space to world
    //         float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

    //         float3 color = _CubeMap.Sample(_CubeMapSampler, sampleVec).rgb * cos(theta) * sin(theta);
    //         irradiance += color;
    //         //irradiance += GammaToLinear(color);
    //         nrSamples++;
    //     }
    // }
    // irradiance = PI * irradiance * (1.0 / float(nrSamples));

    return float4(irradiance, 1);
}

#endif


