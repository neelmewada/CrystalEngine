#include "Core/Macros.hlsli"

Texture2D<float4> InputTexture : SRG_PerPass(t0);
SamplerState InputSampler : SRG_PerPass(s1);

// RWTexture for the output cube map
RWTexture2DArray<float4> OutputCubeMap : SRG_PerPass(u2);

cbuffer _Data : SRG_PerPass(b3)
{
    uint _OutputResolution;
};

// Convert 3D Cartesian coordinates to polar coordinates (latitude and longitude)
float2 CartesianToPolar(float3 cartesian)
{
    float phi = atan2(cartesian.z, cartesian.x);
    float theta = acos(cartesian.y / length(cartesian));
    return float2(phi, theta);
}

// Convert polar coordinates (latitude and longitude) to 3D Cartesian coordinates
float3 PolarToCartesian(float2 polar)
{
    float sinTheta = sin(polar.y);
    return float3(sinTheta * cos(polar.x), cos(polar.y), sinTheta * sin(polar.x));
}

#define NUM_THREADS 16

[numthreads(NUM_THREADS, NUM_THREADS, 1)]
void CSMain(uint3 threadId : SV_DispatchThreadID)
{
    uint faceIndex = threadId.z;
    uint2 pixelCoords = threadId.xy;

    float3 direction = normalize(float3(
        (float(threadId.x) + 0.5) / _OutputResolution * 2 - 1,
        (float(threadId.y) + 0.5) / _OutputResolution * 2 - 1,
        1.0));

    float2 polarCoords = CartesianToPolar(direction);

    float u = polarCoords.x / (2 * PI) + 0.5;
    float v = polarCoords.y / PI + 0.5;

    float4 hdrColor = InputTexture.Sample(InputSampler, float2(u, v));

    OutputCubeMap[pixelCoords.xy, faceIndex] = hdrColor;
}

