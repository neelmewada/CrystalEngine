
#include "Core/Macros.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/Gamma.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
};

BEGIN_ROOT_CONSTANTS()
float4x4 _Transform;
END_ROOT_CONSTANTS()

#if VERTEX

PSInput VertMain(VSInput input)
{
	PSInput o;
    
    o.position = mul(mul(float4(input.position, 1.0), ROOT_CONSTANT(_Transform)), viewProjectionMatrix);
    o.uv = input.uv;
    o.color = input.color;
    return o;
}

#endif

#if FRAGMENT

Texture2D _Texture : SRG_PerDraw(t0);
SamplerState _TextureSampler : SRG_PerDraw(s1);

float4 FragMain(PSInput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}

#endif
