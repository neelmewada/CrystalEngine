
#include "Core/Macros.hlsli"

#include "Core/Macros.hlsli"
#include "Core/Gamma.hlsli"


struct VSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

#if VERTEX

PSInput VertMain(VSInput input)
{
    PSInput o;
    o.position = float4(input.position, 1.0);
    o.uv = input.uv;
    return o;
}

#endif

#if FRAGMENT

Texture2D<float4> _InputTexture : SRG_PerPass(t0);
SamplerState _InputSampler : SRG_PerPass(s1);

float4 FragMain(PSInput input) : SV_TARGET
{
    float3 texColor = _InputTexture.Sample(_InputSampler, input.uv).rgb;
    float gray = dot(float3(0.2126, 0.7152, 0.0722), texColor);
    if (isnan(gray))
        gray = 0;
    return float4(gray, gray, gray, 1.0);
}
#endif
