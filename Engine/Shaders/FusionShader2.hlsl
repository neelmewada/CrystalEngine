
#include "Core/Macros.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/Gamma.hlsli"

#define MAX_CLIP_RECTS 24

struct VSInput
{
    float2  position : POSITION;
    float2  uv : TEXCOORD0;
    float4  color : COLOR0;
    uint    drawType : TEXCOORD1;
    uint    instanceId : SV_INSTANCEID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 globalPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float4 color : TEXCOORD2;
    nointerpolation uint drawType : TEXCOORD3;
    float2 clipPos : TEXCOORD4;
};

#define InstanceIdx input.instanceId

struct ObjectData
{
	float4x4 transform;
};

struct ClipRect
{
	float4x4 clipRectTransform;
    float2 clipRectSize;
};

StructuredBuffer<ObjectData> _Objects : SRG_PerObject(t0);
StructuredBuffer<ClipRect> _ClipRects : SRG_PerObject(t1);

BEGIN_ROOT_CONSTANTS()
uint numClipRects;
uint clipRectIndices[MAX_CLIP_RECTS];
END_ROOT_CONSTANTS()

// Draw features:
// Fill: Solid Color, Texture, Linear Gradient
// Stroke: Straight, Arc, Bezier (Cubic & Quadratic) | Solid, Dashed, Dotted
// Custom shader fill?

#if VERTEX

PSInput VertMain(VSInput input)
{
	PSInput o;

    o.globalPos = mul(float4(input.position.x, input.position.y, 0.0, 1.0), _Objects[input.instanceId].transform).xy;
    o.position = mul(float4(o.globalPos, 0.0, 1.0), viewProjectionMatrix);
    o.uv = input.uv;
    o.color = input.color;
    o.drawType = input.drawType;
    if (ROOT_CONSTANT(numClipRects) > 0)
    {
	    o.clipPos = mul(float4(o.globalPos.xy, 0, 1.0), _ClipRects[ROOT_CONSTANT(clipRectIndices[0])].clipRectTransform).xy;
    }
    else
    {
	    o.clipPos = o.globalPos;
    }
    return o;
}

#endif

#if FRAGMENT

Texture2D<float> _FontAtlas : SRG_PerMaterial(t0);
SamplerState _FontAtlasSampler : SRG_PerMaterial(t1);

Texture2DArray<float4> _Texture : SRG_PerDraw(t0);
SamplerState _TextureSampler : SRG_PerDraw(s1);

/// Credit: https://iquilezles.org/articles/distfunctions2d/
float SDFClipRect(in float2 p, in float2 shapePos, in float2 shapeSize)
{
    p -= float2(shapePos.x + shapeSize.x * 0.5, shapePos.y + shapeSize.y * 0.5);
    float2 d = abs(p) - shapeSize * 0.5;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float4 FragMain(PSInput input) : SV_TARGET
{
	float4 color = input.color;
    float finalAlpha = 1.0;

    for (int i = 0; i < min(ROOT_CONSTANT(numClipRects), MAX_CLIP_RECTS); ++i)
    {
	    int clipIndex = ROOT_CONSTANT(clipRectIndices[i]);

        const float2 clipPos = (i == 0)
    		? input.clipPos
    		: mul(float4(input.globalPos.xy, 0, 1.0), _ClipRects[clipIndex].clipRectTransform).xy;

        
    }

	switch (input.drawType)
	{
	case 1: // Font glyph
		{
            float alpha = _FontAtlas.Sample(_FontAtlasSampler, input.uv).r;
			color = float4(input.color.rgb, input.color.a * alpha);
		}
        break;
    default: // Just raw vertex color
		break;
	}

    return float4(color.rgb, color.a * finalAlpha);
}

#endif
