
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
    None = 0,
	Rect,
    RoundedRect,
    Circle
};

enum BrushType : uint
{
    BRUSH_None = 0,
    BRUSH_Solid,
    BRUSH_Texture,
    BRUSH_LinearGradient
};

enum PenType : uint
{
    PEN_None = 0,
    PEN_SolidLine
};

struct ClipItem2D
{
    float4x4 clipTransform;
    float4 cornerRadius;
    float2 size;
    ShapeType shape;
};

struct ShapeItem2D
{
    float4 cornerRadius;
    float4 brushColor;
    BrushType brushType;
    ShapeType shape;
    PenType penType;
};

// Could be a shape, font character, or any other draw item
struct DrawItem2D
{
    float4x4 transform;
    float4 penColor;
    float2 quadSize;
    float penThickness;
    float opacity;
    DrawType drawType;
    uint shapeOrFontIndex;
    uint charIndex;
    int clipIndex;
};

///////////////////////////////////////////////////////////
/// Shader Resources

StructuredBuffer<DrawItem2D> _DrawList : SRG_PerDraw(t0);
StructuredBuffer<ClipItem2D> _ClipItems : SRG_PerDraw(t1);
StructuredBuffer<ShapeItem2D> _ShapeDrawList : SRG_PerDraw(t2);

///////////////////////////////////////////////////////////
/// Vertex Shader

PSInput VertMain(VSInput input)
{
    PSInput o;
    o.instanceId = input.instanceId;
    o.globalPos = mul(float4(input.position, 1.0), _DrawList[InstanceIdx].transform).xyz;
    const int clipIndex = _DrawList[InstanceIdx].clipIndex;
    if (clipIndex >= 0)
    {
	    o.clipPos = mul(float4(o.globalPos, 1.0), _ClipItems[clipIndex].clipTransform).xyz;
    }
    else
    {
        o.clipPos = o.globalPos;
    }
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
float SDFClipRoundedRect(in float2 p, in float2 shapePos, in float2 shapeSize, in float4 r)
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

float SDFCircle(in float2 p, in float2 shapeSize)
{
    const float r = min(shapeSize.x, shapeSize.y) * 0.5;
    return length(p) - r;
}

// Credit: https://www.ronja-tutorials.com/post/034-2d-sdf-basics/
float SDFRect(in float2 p, in float2 shapeSize)
{
    const float2 componentWiseEdgeDistance = abs(p) - shapeSize * 0.5;
    const float outsideDistance = length(max(componentWiseEdgeDistance, 0));
    const float insideDistance = min(max(componentWiseEdgeDistance.x, componentWiseEdgeDistance.y), 0);
    return outsideDistance + insideDistance;
}

float4 FragMain(PSInput input) : SV_TARGET
{
    const float2 uv = input.uv;
    const float2 quadSize = _DrawList[InstanceIdx].quadSize;
    const float2 sdfPos = (uv - float2(0.5, 0.5)) * quadSize;
    const float2 pos = input.globalPos.xy;
    const float2 clipPos = input.clipPos.xy;
    const int clipIndex = _DrawList[InstanceIdx].clipIndex;

    float clipSdf = -1;

    if (clipIndex >= 0)
    {
        clipSdf = 1;
        const float4 r = _ClipItems[clipIndex].cornerRadius;

	    switch (_ClipItems[clipIndex].shape)
	    {
	    case ShapeType::None:
            clipSdf = -1;
            break;
	    case ShapeType::Rect:
            clipSdf = SDFClipRect(clipPos, float2(0, 0), _ClipItems[clipIndex].size);
		    break;
	    case ShapeType::RoundedRect:
            clipSdf = SDFClipRoundedRect(clipPos, float2(0, 0), _ClipItems[clipIndex].size, float4(r.z, r.y, r.w, r.x));
		    break;
	    case ShapeType::Circle:
            clipSdf = SDFClipCircle(clipPos, float2(0, 0), _ClipItems[clipIndex].size);
		    break;
	    }

        if (clipSdf >= 0) // Outside clip rect
        {
            discard;
        }
    }

    const float clipLerpFactor = clamp(-clipSdf, 0, 1);

    float4 pixelColor = float4(1, 1, 1, 1);

    if (_DrawList[InstanceIdx].drawType == DRAW_Shape)
    {
        const uint shapeIndex = _DrawList[InstanceIdx].shapeOrFontIndex;
        const ShapeItem2D shapeItem = _ShapeDrawList[shapeIndex];

        float sd = 1;

        switch (shapeItem.shape)
        {
            case ShapeType::None:
                discard;
            case ShapeType::Rect:
                sd = SDFRect(sdfPos, quadSize);
                break;
            case ShapeType::RoundedRect:
                break;
            case ShapeType::Circle:
                sd = SDFCircle(sdfPos, quadSize);
                break;
        }

        pixelColor = lerp(float4(shapeItem.brushColor.rgb, 0), shapeItem.brushColor, -sd);
    }

    return float4(pixelColor.rgb, pixelColor.a * clipLerpFactor);
}
