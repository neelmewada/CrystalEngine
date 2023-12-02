//#define SRG(setNum, bindingNum) [[vk::binding(bindingNum, setNum)]]


#include "Macros.hlsl"

//*****************************************************************************************************************************************

struct VertexInfo
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv0 : TEXCOORD0;
};

struct Varyings
{
    float4 position : SV_POSITION;
};

struct CameraData
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 viewProjectionMatrix;
};

struct ModelData
{
    float4x4 modelMatrix;
};

ConstantBuffer<CameraData> _Camera : SRG_PerView(b);

ConstantBuffer<ModelData> _Model : SRG_PerObject(b);

cbuffer _Material : SRG_PerMaterial(b)
{
    float4 albedo;
    float roughness;
    float normalStrength;
    float metallic;
};

Texture2D _AlbedoTex : SRG_PerMaterial(t);

Varyings VertMain(VertexInfo input)
{
    Varyings o;
    o.position = mul(mul(mul(float4(input.position, 1.0), _Model.modelMatrix), _Camera.viewMatrix), _Camera.projectionMatrix);
    return o;
}

float4 FragMain(Varyings input) : SV_TARGET
{
    return albedo * metallic;
}
