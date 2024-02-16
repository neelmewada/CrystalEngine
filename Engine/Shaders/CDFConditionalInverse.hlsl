
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

Texture2D<float> _InputTexture : SRG_PerPass(t0);
SamplerState _InputSampler : SRG_PerPass(s1);

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
    uint inputImageWidth;
    uint inputImageHeight;
    _InputTexture.GetDimensions(inputImageWidth, inputImageHeight);

    float sum = 0.0;
    int y = round2Integer(t2p(input.uv.y, inputImageHeight));
    float u = input.uv.x;
    int x;
    for(x = 0; x < inputImageWidth; x++) {
        float gray = _InputTexture[int2(x, y)].r;
        sum += gray / float(inputImageWidth);
        if(sum >= u) {
            break;
        }
    }
    float cdfXInv = p2t(float(x), inputImageWidth);

    return float4(cdfXInv, cdfXInv, cdfXInv, 1.0);
}
#endif
