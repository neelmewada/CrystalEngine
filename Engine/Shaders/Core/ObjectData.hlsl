#ifndef __OBJECT_DATA_HLSL__
#define __OBJECT_DATA_HLSL__

cbuffer _ObjectData : SRG_PerObject(b)
{
    float4x4 modelMatrix;
};

#endif // __OBJECT_DATA_HLSL__
