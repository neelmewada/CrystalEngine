
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

float2 SampleSphericalMap(float3 v)
{
    const float2 invAtan = float2(0.1591, 0.3183);
    
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

#if FRAGMENT

Texture2D<float4> _InputTexture : SRG_PerPass(t1);
SamplerState _InputSampler : SRG_PerPass(t2);

float4 FragMain(PSInput input) : SV_Target
{
    float2 uv = SampleSphericalMap(normalize(input.localPosition));
    float3 color = _InputTexture.Sample(_InputSampler, uv).rgb;

    return float4(color, 1);
}

#endif
