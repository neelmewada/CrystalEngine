#ifndef __OBJECT_DATA_HLSL__
#define __OBJECT_DATA_HLSL__

#ifdef USE_INSTANCING

struct ObjectData
{
    float4x4 modelMatrix;
};

StructuredBuffer<ObjectData> _ObjectData : SRG_PerDraw(t);

#define MODEL_MATRIX(input) _ObjectData[input.instanceId].modelMatrix
#define INSTANCING() uint instanceId : SV_InstanceID;

#else // USE_INSTANCING

struct ObjectData
{
    float4x4 modelMatrix;
};

ConstantBuffer<ObjectData> _ObjectData : SRG_PerDraw(b);

#define MODEL_MATRIX(input) _ObjectData.modelMatrix
#define INSTANCING()

#endif

#define LOCAL_TO_CLIP_SPACE(position, input) mul(mul(float4(position, 1.0), MODEL_MATRIX(input)), viewProjectionMatrix)

#endif // __OBJECT_DATA_HLSL__
