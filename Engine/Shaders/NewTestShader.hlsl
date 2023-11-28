//#define SRG(setNum, bindingNum) [[vk::binding(bindingNum, setNum)]]


#include "Macros.hlsl"

//*****************************************************************************************************************************************

// data structure : before vertex shader (mesh info)
struct VertexInfo
{
    float3 position : POSITION;
};

// data structure : vertex shader to pixel shader
// also called interpolants because values interpolates through the triangle
// from one vertex to another
struct v2p
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : TEXCOORD1;
};

struct SceneBuffer
{
    float3 lightColor;
    float3 cameraPos;
};

struct ObjectData
{
    float4 model;
};

struct DrawData
{
    int index;
};

ConstantBuffer<SceneBuffer> buff : SRG_PerScene(b);
ConstantBuffer<SceneBuffer> buffer2 : SRG_PerScene(b);

cbuffer buffer3 : SRG_PerScene(b)
{
    float3 ambientColor;
};

StructuredBuffer<ObjectData> _Object : SRG_PerPass(t);

Texture2D _MainTex[10] : SRG_PerMaterial(t);
RWTexture2D<float4> _DataTex : SRG_PerMaterial(u);
SamplerState _MainTexSampler : SRG_PerMaterial(t);

ConstantBuffer<DrawData> _DrawData : SRG_PerDraw(b);

// vertex shader function
v2p VertMain(VertexInfo input)
{
    v2p output;
    output.position = float4(_Object[0].model.rgb * input.position * buff.cameraPos * buffer2.lightColor * ambientColor, 1.0);
    output.uv = float2(0, 0);
    output.color = float3(1, 1, 0);
    return output;
}

// pixel shader function
float4 FragMain(v2p input) : SV_TARGET
{
    float4 col = _MainTex[_DrawData.index].Sample(_MainTexSampler, float2(0, 0));
    _DataTex[int2(1, 1)] = float4(0.5, 0.5, 0.5, 1);
    return float4(buff.lightColor * col.rgb * ambientColor, 0.0);
}
