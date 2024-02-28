
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

Texture2D<float> _InputA : SRG_PerPass(t0);
Texture2D<float> _InputB : SRG_PerPass(t1);
SamplerState _InputSampler : SRG_PerPass(s2);

float p2t(in float p, in int noOfPixels) {
   return (p + 0.5) / float(noOfPixels);
}

float t2p(in float t, in int noOfPixels) {
   return t * float(noOfPixels) - 0.5;
}

int round2Integer(in float val) {
  return int(val + 0.5);
}

float4 FragMain(PSInput input) : SV_TARGET
{
    float a = _InputA.Sample(_InputSampler, input.uv).r;
    float b = _InputB.Sample(_InputSampler, input.uv).r;
    float c = 0.0;
    if(abs(b) > 1e-8) { // prevent division by zero
        c = a / b;
    }
    return float4(c, c, c, 1.0);
}
#endif
