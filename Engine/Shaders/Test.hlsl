
#include "Core/Macros.hlsli"

struct PointLightData
{
    float3 worldPos;
    float radius;
    float intensity;
    float3 _padding;
};

StructuredBuffer<PointLightData> _PointLights : SRG_PerPass(t);

Texture2D _Tex : SRG_PerPass(t);
SamplerState _Sampler : SRG_PerPass(s);

struct VertexInput
{
    float3 position : POSITION;
};

struct Varyings
{
    float4 position : SV_POSITION;
};

BEGIN_ROOT_CONSTANTS()
    uint numPointLights;
END_ROOT_CONSTANTS()

Varyings VertMain(VertexInput input)
{
    Varyings o;
    o.position = float4(input.position, 1.0);
    return o;
}

float4 FragMain(Varyings input) : SV_TARGET
{
    //_Tex[uint2(1, 2)] = 2;
    uint w, h;
    _Tex.GetDimensions(w, h);
    // _Tex.Sample(_Sampler, float2(0.5, 0.5))
    return float4(_PointLights[ROOT_CONSTANT(numPointLights) - 1].worldPos, _Tex.Sample(_Sampler, float2(0.5, 0.5)).r * w);
}

