#ifndef __VIEW_DATA_HLSL__
#define __VIEW_DATA_HLSL__

cbuffer _PerViewData : SRG_PerView(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
    float4 viewPosition;
    float2 pixelResolution;
};

#endif // __VIEW_DATA_HLSL__
