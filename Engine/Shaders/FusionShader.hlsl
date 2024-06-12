
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
    DRAW_Texture
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
struct DrawItem2D
{
    float4x4 transform;
    float4 penColor;
    float2 quadSize;
    float penThickness;
    float opacity;
    DrawType drawType;
    uint shapeIndex;
    uint fontIndex;
    uint charIndex;
    int clipIndex;
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
    o.position = mul(mul(float4(input.position, 1.0), _DrawList[InstanceIdx].transform), viewProjectionMatrix);
    o.uv = input.uv;
    return o;
}

///////////////////////////////////////////////////////////
/// Fragment Shader

// - SDF Functions -

float SDFRect(float2 p)
{
    return 0;
}

struct RenderData
{
	
};

float4 FragMain(PSInput input) : SV_TARGET
{
    const float2 uv = input.uv;
    const float2 scaledUV = uv * _DrawList[InstanceIdx].quadSize;

    if (_DrawList[InstanceIdx].clipIndex >= 0)
    {
	    // TODO: Check for clipping
    }

    switch (_DrawList[InstanceIdx].drawType)
    {
    case DRAW_Shape:
        break;
    }

    return float4(uv.x, uv.y, 0, 1);
}
