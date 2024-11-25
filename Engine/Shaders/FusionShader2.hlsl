
#include "Core/Macros.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/Gamma.hlsli"

struct VSInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    uint index : TEXCOORD1;
    uint instanceId : SV_INSTANCEID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
};

#define InstanceIdx input.instanceId

struct ObjectData
{
	float4x4 transform;
};

StructuredBuffer<ObjectData> _Objects : SRG_PerObject(t0);

// Draw features:
// Fill: Solid Color, Texture, Linear Gradient
// Stroke: Straight, Arc, Bezier (Cubic & Quadratic) | Solid, Dashed, Dotted
// Custom shader fill?

#if VERTEX

PSInput VertMain(VSInput input)
{
	PSInput o;
    
    o.position = mul(mul(float4(input.position.x, input.position.y, 0.0, 1.0), _Objects[input.instanceId].transform), viewProjectionMatrix);
    o.uv = input.uv;
    o.color = input.color;
    return o;
}

#endif

#if FRAGMENT

Texture2D<float> _FontAtlas : SRG_PerMaterial(t0);
SamplerState _FontAtlasSampler : SRG_PerMaterial(t1);

Texture2DArray<float4> _Texture : SRG_PerDraw(t0);
SamplerState _TextureSampler : SRG_PerDraw(s1);

float4 FragMain(PSInput input) : SV_TARGET
{
    return input.color;
}

#endif
