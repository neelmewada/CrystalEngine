#ifndef __OBJECT_DATA_HLSL__
#define __OBJECT_DATA_HLSL__

#pragma feature USE_INSTANCING

#ifdef USE_INSTANCING

struct ObjectData
{
    float4x4 modelMatrix;
};

StructuredBuffer<ObjectData> _ObjectData : SRG_PerObject(t);

#define MODEL_MATRIX(input) _ObjectData[input.instanceId].modelMatrix
#define INSTANCING() uint instanceId : SV_InstanceID;

#else // USE_INSTANCING

struct ObjectData
{
    float4x4 modelMatrix;
};

ConstantBuffer<ObjectData> _ObjectData : SRG_PerObject(b);

#define MODEL_MATRIX(input) _ObjectData.modelMatrix
#define INSTANCING()

#endif

#define LOCAL_TO_CLIP_SPACE(localSpace, vertexInput) mul(mul(localSpace, MODEL_MATRIX(vertexInput)), viewProjectionMatrix)

#define LOCAL_TO_WORLD_SPACE(localSpace, vertexInput) mul(localSpace, MODEL_MATRIX(vertexInput))


#endif // __OBJECT_DATA_HLSL__
