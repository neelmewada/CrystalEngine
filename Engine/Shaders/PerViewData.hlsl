#ifndef __PER_VIEW_DATA_HLSL__
#define __PER_VIEW_DATA_HLSL__

cbuffer _PerViewData : SRG_PerView(b)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
};

#endif // __PER_VIEW_DATA_HLSL__
