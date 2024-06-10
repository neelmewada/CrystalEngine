
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
    nointerpolation uint instanceId : TEXCOORD1;
};

#define instanceIdx input.instanceId

///////////////////////////////////////////////////////////
/// Data Structures

enum DrawType
{
	DRAW_Rect
};

struct DrawItem2D
{
    float4x4 transform;
    DrawType drawType;
    
};

///////////////////////////////////////////////////////////
/// Shader Resources

StructuredBuffer<DrawItem2D> _DrawList : SRG_PerDraw(t0);

///////////////////////////////////////////////////////////
/// Vertex Shader

PSInput VertMain(VSInput i)
{
    PSInput o;
    o.instanceId = i.instanceId;
    o.position = float4(i.position, 1.0);
    o.uv = i.uv;
    return o;
}

///////////////////////////////////////////////////////////
/// Fragment Shader

struct RenderData
{
	
};

float4 RenderRect(in float4 p)
{
    return float4(0, 0, 0, 0);
}

float4 FragMain(PSInput input) : SV_TARGET
{
    return float4(0, 0, 0, 0);
}
