
#include "Core/Macros.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/Gamma.hlsli"

struct VSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    uint instanceId : SV_INSTANCEID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    nointerpolation uint instanceId : TEXCOORD2;
};

#define instanceIdx input.instanceId

///////////////////////////////////////////////////////////
/// Data Structures

enum DrawType : uint
{
	DRAW_Rect
};

struct DrawItem2D
{
    float4x4 transform;
    DrawType drawType; // enum DrawType
};

///////////////////////////////////////////////////////////
/// Shader Resources

StructuredBuffer<DrawItem2D> _DrawList : SRG_PerDraw(t0);

///////////////////////////////////////////////////////////
/// Vertex Shader

PSInput VertMain(VSInput input)
{
    PSInput o;
    o.instanceId = input.instanceId;
    o.position = mul(mul(float4(input.position, 1.0), _DrawList[instanceIdx].transform), viewProjectionMatrix);
    o.uv = input.uv;
    return o;
}

///////////////////////////////////////////////////////////
/// Fragment Shader

struct RenderData
{
	
};

float4 FragMain(PSInput input) : SV_TARGET
{
    const float2 uv = input.uv;

    return float4(uv.x, uv.y, 0, 1);
}
