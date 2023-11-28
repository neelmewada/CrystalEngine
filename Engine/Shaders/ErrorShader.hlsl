
#ifndef PerScene_Frequency
#define PerScene_Frequency 0
#endif

#ifndef PerView_Frequency
#define PerView_Frequency 0
#endif

#ifndef PerPass_Frequency
#define PerPass_Frequency 1
#endif

#ifndef PerMaterial_Frequency
#define PerMaterial_Frequency 2
#endif

#ifndef PerObject_Frequency
#define PerObject_Frequency 3
#endif

#ifndef PerDraw_Frequency
#define PerDraw_Frequency 3
#endif

#define EXPAND(x) x
#define CONCATENATE(arg1, arg2)   __CONCATENATE1(arg1, arg2)
#define __CONCATENATE1(arg1, arg2)  __CONCATENATE2(arg1, arg2)
#define __CONCATENATE2(arg1, arg2)  arg1 ## arg2

#define SRG(frequencyId, type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, frequencyId)))

#define SRG_PerScene(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerScene_Frequency)))
#define SRG_PerView(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerView_Frequency)))
#define SRG_PerPass(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerPass_Frequency)))
#define SRG_PerMaterial(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerMaterial_Frequency)))
#define SRG_PerObject(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerObject_Frequency)))
#define SRG_PerDraw(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerDraw_Frequency)))

// data structure : before vertex shader (mesh info)
struct VertexInfo
{
    float3 position : POSITION;
};

struct v2f
{
    float4 position : SV_POSITION;
};

struct PerViewData
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
};

struct ModelData
{
    float4x4 modelMatrix;
};

ConstantBuffer<PerViewData> _PerViewData : register(b0, space0);

ConstantBuffer<ModelData> _Model : register(b0, space1);

// vertex shader function
v2f VertMain(VertexInfo input)
{
    v2f output;
    //output.position = mul(float4(input.position, 1.0), mul(_PerViewData.projectionMatrix, mul(_PerViewData.viewMatrix, _Model.modelMatrix)));
    output.position = mul(mul(mul(float4(input.position, 1.0), _Model.modelMatrix), _PerViewData.viewMatrix), _PerViewData.projectionMatrix);
    return output;
}

// pixel shader function
float4 FragMain(v2f input) : SV_TARGET
{
    return float4(1.0, 0, 1.0, 1.0);
}

