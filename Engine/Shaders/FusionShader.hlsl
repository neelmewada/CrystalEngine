
#include "Core/Macros.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/Gamma.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    uint instanceId : SV_INSTANCEID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    nointerpolation uint instanceId : TEXCOORD1;
};

PSInput VertMain(VSInput i)
{
    PSInput o;
    o.instanceId = i.instanceId;
    o.position = float4(i.position, 1.0);
    o.uv = i.uv;
    return o;
}

float4 FragMain(PSInput input) : SV_TARGET
{
    return float4(0, 0, 0, 0);
}
