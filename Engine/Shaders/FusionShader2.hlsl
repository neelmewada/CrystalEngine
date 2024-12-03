
#include "Core/Macros.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/Gamma.hlsli"

#define MAX_CLIP_RECTS 24

enum FDrawType : int
{
    DRAW_Geometry = 0,
    DRAW_Text,
    DRAW_TextureNoTile,
    DRAW_TextureTileX,
    DRAW_TextureTileY,
    DRAW_TextureTileXY,
};

enum class FImageFit : int
{
	None = 0,
    Fill,
    Contain,
    Cover
};

struct VSInput
{
    float2  position : POSITION;
    float2  uv : TEXCOORD0;
    float4  color : COLOR0;
    int     drawType : TEXCOORD1;
    int     index	 : TEXCOORD2;
    uint    instanceId : SV_INSTANCEID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 globalPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float4 color : TEXCOORD2;
    nointerpolation int drawType : TEXCOORD3;
    float2 clipPos : TEXCOORD4;
    nointerpolation int index : TEXCOORD5;
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

struct DrawData
{
    float2 rectSize;
	float2 uvMin;
    float2 uvMax;
    float2 brushPos;
    float2 brushSize;
    // Index into texture Array
    int index;
    FImageFit imageFit;
};

StructuredBuffer<ObjectData> _Objects : SRG_PerObject(t0);
StructuredBuffer<ClipRect> _ClipRects : SRG_PerObject(t1);
StructuredBuffer<DrawData> _DrawData : SRG_PerObject(t2);

BEGIN_ROOT_CONSTANTS()
float2 transparentUV;
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
    o.index = input.index;
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
    float clipSdf = -1;
    const float2 transparentUV = ROOT_CONSTANT(transparentUV);
    const float2 inputUV = input.uv;

    for (int i = 0; i < min(ROOT_CONSTANT(numClipRects), MAX_CLIP_RECTS); ++i)
    {
	    int clipIndex = ROOT_CONSTANT(clipRectIndices[i]);

        const float2 clipPos = (i == 0)
    		? input.clipPos
    		: mul(float4(input.globalPos.xy, 0, 1.0), _ClipRects[clipIndex].clipRectTransform).xy;

        float sd = SDFClipRect(clipPos, float2(0, 0), _ClipRects[clipIndex].clipRectSize);

        if (sd > 0) // Outside clip rect
        {
	        discard;
        }

        clipSdf = max(clipSdf, sd);
    }
    
	switch ((FDrawType)input.drawType)
	{
	case DRAW_Text: // Font glyph
		{
            float alpha = _FontAtlas.Sample(_FontAtlasSampler, inputUV).r;
			color = float4(input.color.rgb, input.color.a * alpha);
		}
        break;
    case DRAW_TextureNoTile: // Draw texture
    case DRAW_TextureTileX:
    case DRAW_TextureTileY:
    case DRAW_TextureTileXY:
	    {
            // TODO: Add tiling support
			const DrawData drawData = _DrawData[input.index];
            int layerIndex = drawData.index;

            // Normalized value of start and end position
            float2 uvStartPos = (drawData.rectSize - drawData.brushSize) * drawData.brushPos / drawData.rectSize;
            float2 uvEndPos = uvStartPos + drawData.brushSize / drawData.rectSize;
            float2 uv = (inputUV - uvStartPos) / (uvEndPos - uvStartPos);

            if ((FDrawType)input.drawType == DRAW_TextureNoTile && (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1))
            {
	            color.a = 0;
            }
            else if ((FDrawType)input.drawType == DRAW_TextureTileX && (uv.y < 0 || uv.y > 1))
            {
	            color.a = 0;
            }
            else if ((FDrawType)input.drawType == DRAW_TextureTileY && (uv.x < 0 || uv.x > 1))
            {
	            color.a = 0;
            }
            else
            {
				if (uv.x < 0)
				{
					uv.x = 1 - (abs(uv.x) % 1);
				}
                else if (uv.x > 1)
                {
	                uv.x = uv.x % 1;
                }
                if (uv.y < 0)
                {
	                uv.y = 1 - (abs(uv.y) % 1);
                }
                else if (uv.y > 1)
                {
	                uv.y = uv.y % 1;
                }

                float2 textureUV = drawData.uvMin + uv * (drawData.uvMax - drawData.uvMin);
                float4 sample = _Texture.Sample(_TextureSampler, float3(textureUV.x, textureUV.y, layerIndex));
                color *= sample;
            }
	    }
		break;
    default:
		break;
	}

    const float clipAlpha = clamp(-clipSdf, 0, 1);

    return float4(color.rgb, color.a * clipAlpha);
}

#endif
