#ifndef __VIEW_DATA_HLSL__
#define __VIEW_DATA_HLSL__

cbuffer _PerViewData : SRG_PerView(b)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
    float3 viewPosition;
};

cbuffer _CameraData : SRG_PerView(b)
{
    float3 cameraPosition;
    float fieldOfView;
};

#endif // __VIEW_DATA_HLSL__
