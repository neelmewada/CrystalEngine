
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
    DRAW_Line,
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
    PEN_SolidLine,
    PEN_DashedLine,
    PEN_DottedLine
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
    float2 uvMin;
    float2 uvMax;
    BrushType brushType;
    ShapeType shape;
    uint textureOrGradientIndex;
    uint samplerIndex;
};

struct LineItem2D
{
    float2 start;
    float2 end;
    float dashLength;
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
    PenType penType;
    uint shapeOrCharOrLineIndex;
    int clipIndex;
};

struct GlyphItem
{
    float4 atlasUV;
    uint mipIndex;
};

///////////////////////////////////////////////////////////
/// Shader Resources

StructuredBuffer<DrawItem2D> _DrawList : SRG_PerDraw(t0);
StructuredBuffer<ClipItem2D> _ClipItems : SRG_PerDraw(t1);
StructuredBuffer<ShapeItem2D> _ShapeDrawList : SRG_PerDraw(t2);
StructuredBuffer<LineItem2D> _LineItems : SRG_PerDraw(t3);

#if FRAGMENT

Texture2D<float> _FontAtlas : SRG_PerMaterial(t0);
StructuredBuffer<GlyphItem> _GlyphItems : SRG_PerMaterial(t1);
SamplerState _FontAtlasSampler : SRG_PerMaterial(s2);

#define MAX_TEXTURE_COUNT 100000
#define MAX_SAMPLER_COUNT 128

Texture2D _Textures[MAX_TEXTURE_COUNT] : SRG_PerObject(t0);
SamplerState _TextureSamplers[MAX_SAMPLER_COUNT] : SRG_PerObject(s1);

#endif

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
        o.clipPos = mul(float4(o.globalPos.xy, 0, 1.0), _ClipItems[clipIndex].clipTransform).xyz;
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

// Credit: https://iquilezles.org/articles/distfunctions2d/
// r.x = roundness bottom-right  
// r.y = roundness top-right
// r.z = roundness bottom-left
// r.w = roundness top-left
float SDFRoundedRect(in float2 p, in float2 shapeSize, in float4 r)
{
    r.xy = (p.x > 0.0) ? r.xy : r.zw;
    r.x = (p.y > 0.0) ? r.x : r.y;
    float2 q = abs(p) - shapeSize * 0.5 + r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}

// Credit: https://iquilezles.org/articles/distfunctions2d/
float SDFSegment(in float2 p, in float2 a, in float2 b)
{
    float2 ba = b - a;
    float2 pa = p - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - h * ba);
}

// Signed distance function for a dashed line segment
float SDFDashedLine(float2 p, float2 a, float2 b, float dashLength, float thickness)
{
    float2 ba = b - a;
    float totalLength = length(ba);
    ba = ba / totalLength;
    float t = dot(p - a, ba) / totalLength;
    float pattern = fmod(t, dashLength * 2.0);
    float dashMask = step(pattern, dashLength);
    float d = SDFSegment(p, a, b) - thickness;
    return lerp(1.0, d, dashMask);
}

// -----------------------------------------------------------------------------------------

#if FRAGMENT

float4 FragMain(PSInput input) : SV_TARGET
{
    const float2 uv = input.uv;
    const DrawItem2D drawItem = _DrawList[InstanceIdx];
    const float2 quadSize = drawItem.quadSize;
    const float2 sdfPos = (uv - float2(0.5, 0.5)) * quadSize;
    const float2 pos = input.globalPos.xy;
    const float2 clipPos = input.clipPos.xy;
    const int clipIndex = drawItem.clipIndex;
    const float4 penColor = drawItem.penColor;
    float penThickness = drawItem.penThickness;

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

    if (drawItem.drawType == DRAW_Shape)
    {
        const uint shapeIndex = drawItem.shapeOrCharOrLineIndex;
        const ShapeItem2D shapeItem = _ShapeDrawList[shapeIndex];
        const uint samplerIndex = shapeItem.samplerIndex;
        const uint textureIndex = shapeItem.textureOrGradientIndex;
        const uint gradientIndex = textureIndex;
        const float4 r = shapeItem.cornerRadius;
        const float2 uvMin = shapeItem.uvMin;
        const float2 uvMax = shapeItem.uvMax;
        const float2 textureUV = float2((uv.x - uvMin.x) / (uvMax.x - uvMin.x), (uv.y - uvMin.y) / (uvMax.y - uvMin.y));

        float sd = 1;

        switch (shapeItem.shape)
        {
            case ShapeType::None:
                discard;
            case ShapeType::Rect:
                sd = SDFRect(sdfPos, quadSize);
                break;
            case ShapeType::RoundedRect:
                sd = SDFRoundedRect(sdfPos, quadSize, float4(r.z, r.y, r.w, r.x));
                break;
            case ShapeType::Circle:
                sd = SDFCircle(sdfPos, quadSize);
                break;
        }

        float4 color = float4(0, 0, 0, 0);

        switch (shapeItem.brushType)
        {
        case BRUSH_None:
	        break;
        case BRUSH_Solid:
            color = shapeItem.brushColor;
	        break;
        case BRUSH_Texture:
            color = _Textures[textureIndex].Sample(_TextureSamplers[samplerIndex], textureUV).rgba * shapeItem.brushColor;
	        break;
        case BRUSH_LinearGradient:
	        break;
        }

        if (penThickness > 0 && penColor.a > 0)
        {
            float borderSdf = abs(sd + penThickness) - penThickness;
            color = lerp(color, penColor, clamp(-borderSdf * 2.5, 0, 1));
        }

        // Lerp fillColor with SDF for anti-aliased edges
        pixelColor = lerp(float4(color.rgb, 0), color, -sd * 5);
    }
    else if (drawItem.drawType == DRAW_Line)
    {
        const uint lineIndex = drawItem.shapeOrCharOrLineIndex;
        const LineItem2D lineItem = _LineItems[lineIndex];
        float dist = length(lineItem.end - lineItem.start);
        const float2 endPos = lineItem.end;
        const float2 startPos = lineItem.start;

        float sd = 1;

        switch (drawItem.penType)
        {
        case PEN_None:
            discard;
        case PEN_SolidLine:
            sd = SDFSegment(uv * quadSize, startPos, endPos) - penThickness;
	        break;
        case PEN_DottedLine:
            penThickness = 1.0;
        case PEN_DashedLine:
            sd = SDFDashedLine(sdfPos + quadSize * 0.5, startPos, endPos, lineItem.dashLength / dist, penThickness);
	        break;
        }

        pixelColor = lerp(float4(penColor.rgb, 0), penColor, -sd * 2.0);
    }
    else if (drawItem.drawType == DRAW_Text)
    {
        const uint charIndex = drawItem.shapeOrCharOrLineIndex;
        const GlyphItem glyphItem = _GlyphItems[charIndex];
        const float2 uvMin = glyphItem.atlasUV.xy;
        const float2 uvMax = glyphItem.atlasUV.zw;

        float alpha = _FontAtlas.Sample(_FontAtlasSampler, uvMin + (uvMax - uvMin) * uv).r;

        pixelColor.rgb = penColor.rgb;
        pixelColor.a = penColor.a * alpha * 1.1;
    }

    return float4(pixelColor.rgb, pixelColor.a * clipLerpFactor);
}

#endif
