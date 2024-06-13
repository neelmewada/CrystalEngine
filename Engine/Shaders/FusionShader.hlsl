
#include "Core/Macros.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/Gamma.hlsli"

#ifndef COMPILE
// Only for intellisense purpose! Actual constant buffer is in ViewData.hlsli file
cbuffer _PerViewData : SRG_PerView(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
    float4 viewPosition;
    float2 pixelResolution;
};
#endif

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
    float3 globalPos : TEXCOORD1;
    float3 clipPos : TEXCOORD2;
    nointerpolation uint instanceId : TEXCOORD3;
};

#define InstanceIdx input.instanceId

///////////////////////////////////////////////////////////
/// Data Structures & Types

// TODO: Features
// - Clipping by shape
// - Shapes with Fill and Border
// - Text characters
// - 

enum DrawType : uint
{
	DRAW_Shape,
    DRAW_Texture,
    DRAW_Text
};

enum class ShapeType: uint
{
	Rect,
    RoundedRect,
    Circle
};

enum class BrushType : uint
{
    None,
	Solid,
    Gradient,
    Texture
};

struct ShapeItem2D
{
    float4 cornerRadius;
    float4 brushColor;
    float2 brushUVScaling;
    float2 brushUVOffset;
    uint gradientStartIdx;
    uint gradientEndIdx;
    BrushType brushType;
    ShapeType shapeType;
};

// Could be a shape, font character, or any other draw item
struct DrawItem2DNew
{
    float4x4 transform;
    float4 penColor;
    float2 quadSize;
    float penThickness;
    float opacity;
    DrawType drawType;
    uint shapeOrfontIndex;
    uint charIndex;
    int clipIndex;
};

struct ClipItem2D
{
    float4x4 clipTransform;
    float4 cornerRadius;
    ShapeType shapeType;
};

struct DrawItem2D
{
    float4x4 transform;
    DrawType drawType;
    int clipIndex;
};

///////////////////////////////////////////////////////////
/// Shader Resources

StructuredBuffer<DrawItem2D> _DrawList : SRG_PerDraw(t0);
StructuredBuffer<ClipItem2D> _ClipItems : SRG_PerDraw(t1);

///////////////////////////////////////////////////////////
/// Vertex Shader

PSInput VertMain(VSInput input)
{
    PSInput o;
    o.instanceId = input.instanceId;
    o.globalPos = mul(float4(input.position, 1.0), _DrawList[InstanceIdx].transform).xyz;
    o.clipPos = mul(float4(o.globalPos, 1.0), _ClipItems[0].clipTransform).xyz;
    o.position = mul(float4(o.globalPos, 1.0), viewProjectionMatrix);
    o.uv = input.uv;
    return o;
}

///////////////////////////////////////////////////////////
/// Fragment Shader

// - SDF Functions -

// Credit: https://iquilezles.org/articles/distfunctions2d/
float SDFClipRect(in float2 p, in float2 shapePos, in float2 shapeSize)
{
    p -= float2(shapePos.x + shapeSize.x * 0.5, shapePos.y + shapeSize.y * 0.5);
    float2 d = abs(p) - shapeSize * 0.5;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

// Credit: https://iquilezles.org/articles/distfunctions2d/
// r.x = roundness bottom-right  
// r.y = roundness top-right
// r.z = roundness bottom-left
// r.w = roundness top-left
float SDFClipRoundedBox(in float2 p, in float2 shapePos, in float2 shapeSize, in float4 r)
{
    p -= float2(shapePos.x + shapeSize.x * 0.5, shapePos.y + shapeSize.y * 0.5);

    r.xy = (p.x > 0.0) ? r.xy : r.zw;
    r.x = (p.y > 0.0) ? r.x : r.y;
    float2 q = abs(p) - shapeSize * 0.5 + r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}

float SDFClipCircle(in float2 p, in float2 shapePos, in float2 shapeSize)
{
    p -= float2(shapePos.x + shapeSize.x * 0.5, shapePos.y + shapeSize.y * 0.5);
    const float r = min(shapeSize.x, shapeSize.y) * 0.5;
    return length(p) - r;
}


struct ShapeRenderData
{
	
};

float4 FragMain(PSInput input) : SV_TARGET
{
    const float2 uv = input.uv;
    const float2 pos = input.globalPos.xy;
    const float2 clipPos = input.clipPos.xy;
    //const float2 scaledUV = uv * _DrawList[InstanceIdx].quadSize;

    if (_DrawList[InstanceIdx].clipIndex >= 0)
    {
        float sd = SDFClipRoundedBox(clipPos, float2(0, 0), float2(200, 125), float4(10, 20, 30, 40));

        if (sd >= 0) // Outside clip rect
        {
            return float4(0, 0.5, 0, 1);
        }
    }

    return float4(1, 1, 1, 1);
    return float4(pos.x / pixelResolution.x, pos.y / pixelResolution.y, 0, 1);
}
