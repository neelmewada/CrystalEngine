//#define SRG(setNum, bindingNum) [[vk::binding(bindingNum, setNum)]]

#define SRG(pos, set) register(pos, space##set)

#ifndef PerScene_Frequency
#define PerScene_Frequency 0
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

#define SRG_PerScene(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerScene_Frequency)))
#define SRG_PerPass(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerPass_Frequency)))
#define SRG_PerMaterial(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerMaterial_Frequency)))
#define SRG_PerObject(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerObject_Frequency)))
#define SRG_PerDraw(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerDraw_Frequency)))

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

TextureBuffer<ObjectData> _Object : SRG_PerPass(t);

Texture2D _MainTex[10] : SRG_PerMaterial(t);
RWTexture2D<float4> _DataTex : SRG_PerMaterial(u);
SamplerState _MainTexSampler : SRG_PerMaterial(t);

ConstantBuffer<DrawData> _DrawData : SRG_PerDraw(b);

// vertex shader function
v2p VertMain(VertexInfo input)
{
    v2p output;
    output.position = float4(_Object.model.rgb * input.position * buff.cameraPos * buffer2.lightColor, 1.0);
    output.uv = float2(0, 0);
    output.color = float3(1, 1, 0);
    return output;
}

// pixel shader function
float4 FragMain(v2p input) : SV_TARGET
{
    float4 col = _MainTex[_DrawData.index].Sample(_MainTexSampler, float2(0, 0));
    _DataTex[int2(1, 1)] = float4(0.5, 0.5, 0.5, 1);
    return float4(buff.lightColor * col.rgb, 0.0);
}
