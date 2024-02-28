
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
    int x = round2Integer(t2p(input.uv.x, inputImageWidth));

    for(int y = 0; y < inputImageHeight; y++) {
        float gray = _InputTexture[int2(x, y)].r;
        if (isnan(gray))
            gray = 0;
        sum += gray;
    }
    sum /= float(inputImageHeight);

    return float4(sum, sum, sum, 1.0);
}
#endif
