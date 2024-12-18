Shader "2D/SDF Geometry"
{
    Properties
    {
        _SDFSmoothness("SDF Smoothness", Float) = 50.0
    }

    SubShader
    {
        Tags {
            "Blend" = "SrcAlpha,OneMinusSrcAlpha"
        }

        Pass
        {
            Name "SDF"
            Tags { 
                "Vertex"="VertMain", "Fragment"="FragMain", "Platform"="Desktop"
            }
            ZWrite Off
            ZTest Off
            Cull Off

            HLSLPROGRAM
            
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
                float2 screenPosition : TEXCOORD2;
                float4 uvBounds : TEXCOORD3;
            };

            struct DrawItem
            {
                float4x4 transform;
                float4 fillColor;
                float4 outlineColor;
                float4 cornerRadius;
                float2 itemSize; // item size in pixels
                float borderThickness;
                float dashLength;
                uint drawType; // enum DrawType;
                uint charIndex;
                uint bold; 
                uint clipRect;
                uint textureIndex; // OR gradientStartIdx
                uint samplerIndex; // OR gradientEndIdx
            };

            #define gradientStartIdx textureIndex
            #define gradientEndIdx samplerIndex
            #define gradientRadians dashLength

            StructuredBuffer<DrawItem> _DrawList : SRG_PerDraw(t0);

            struct ClipRect
            {
                float4 rect;
                float4 cornerRadius;
            };

            StructuredBuffer<ClipRect> _ClipRects : SRG_PerDraw(t1);

            #define MAX_SAMPLERS 32

            SamplerState _TextureSamplers[MAX_SAMPLERS] : SRG_PerDraw(s2);

            #define MAX_TEXTURES 100000 // 100,000

            Texture2D<float4> _Textures[MAX_TEXTURES] : SRG_PerDraw(t3);

            cbuffer _DrawDataConstants : SRG_PerDraw(b4)
            {
                uint _FrameIndex;
            };

            struct GradientKey
            {
                float4 color;
                float position;
            };

            StructuredBuffer<GradientKey> _GradientKeys : SRG_PerDraw(t5);

            struct CharacterItem
            {
                float4 atlasUV;
            };

            StructuredBuffer<CharacterItem> _CharacterData : SRG_PerMaterial(t2);

            enum DrawType : uint
            {
                DRAW_None = 0,
                DRAW_Text,
                DRAW_Circle,
                DRAW_Rect,
                DRAW_RoundedRect,
                DRAW_RoundedX,
                DRAW_Texture,
                DRAW_FrameBuffer,
                DRAW_Triangle,
                DRAW_DashedLine,
                DRAW_LinearGradient,
            };

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                o.instanceId = input.instanceId;
                o.uv = input.uv;
                if (_DrawList[input.instanceId].drawType == DRAW_Text) // Text
                {
                    float4 uvBounds = _CharacterData[_DrawList[input.instanceId].charIndex].atlasUV;
                    o.uvBounds = uvBounds;
                    o.uv = float2(lerp(uvBounds.x, uvBounds.z, input.uv.x), lerp(uvBounds.y, uvBounds.w, input.uv.y));
                }

                o.position = mul(float4(input.position, 1.0), _DrawList[input.instanceId].transform);
                o.screenPosition = o.position.xy / 2.0;
                o.position = mul(o.position, viewProjectionMatrix);
                return o;
            }

            #endif

            #if FRAGMENT

            Texture2D<float> _FontAtlas : SRG_PerMaterial(t0);
            SamplerState _FontAtlasSampler : SRG_PerMaterial(s1);

            cbuffer _MaterialData : SRG_PerMaterial(b3)
            {
                float _SDFSmoothness;
            };

            inline float SDFCircle(float2 p, float r)
            {
                return length(p) - r;
            }

            // Credit: https://iquilezles.org/articles/distfunctions2d/
            inline float SDFBox( in float2 p, in float2 b )
            {
                float2 d = abs(p) - b;
                return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
            }
            
            // Credit: https://iquilezles.org/articles/distfunctions2d/
            // b.x = width
            // b.y = height
            // r.x = roundness top-right  
            // r.y = roundness boottom-right
            // r.z = roundness top-left
            // r.w = roundness bottom-left
            inline float SDFRoundRect(in float2 p, in float2 b, in float4 r ) 
            {
                r.xy = (p.x>0.0)?r.xy : r.zw;
                r.x  = (p.y>0.0)?r.x  : r.y;
                float2 q = abs(p)-b+r.x;
                return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r.x;
            }

            // Credit: https://iquilezles.org/articles/distfunctions2d/
            float SDFRoundedCross( in float2 p, in float w, in float r )
            {
                p = abs(p);
                return length(p-min(p.x+p.y,w)*0.5) - r;
            }

            // Credit: https://iquilezles.org/articles/distfunctions2d/
            float SDFRoundedX( in float2 p, in float w, in float r )
            {
                p = abs(p);
                return length(p-min(p.x+p.y,w)*0.5) - r;
            }
               
            float SDFSegment( in float2 p, in float2 a, in float2 b )
            {
                float2 ba = b-a;
                float2 pa = p-a;
                float h =clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
                return length(pa-h*ba);
            }

            // Signed distance function for a dashed line segment
            float SDFDashedLine(float2 p, float2 a, float2 b, float dashLength, float thickness) {
                float2 ba = b - a;
                float totalLength = length(ba);
                ba = normalize(ba);
                float t = dot(p - a, ba) / totalLength;
                float pattern = fmod(t, dashLength * 2.0);
                float dashMask = step(pattern, dashLength);
                float d = SDFSegment(p, a, b) - thickness;
                return lerp(1.0, d, dashMask);
            }

            // Credit: https://iquilezles.org/articles/distfunctions2d/
            float SDFIsoscelesTriangle(in float2 p, in float2 q)
            {
                p.x = abs(p.x);
	            float2 a = p - q*clamp( dot(p,q)/dot(q,q), 0.0, 1.0 );
                float2 b = p - q*float2( clamp( p.x/q.x, 0.0, 1.0 ), 1.0 );
                float k = sign( q.y );
                float d = min(dot( a, a ),dot(b, b));
                float s = max( k*(p.x*q.y-p.y*q.x),k*(p.y-q.y)  );
	            return sqrt(d)*sign(s);
            }

            struct GeometryInfo
            {
                float4 fillColor;
                float4 outlineColor;
                float4 cornerRadius;
                float2 itemSize;
                float2 uv;
                float borderThickness;
                float dashLength;
                uint textureIndex;
                uint samplerIndex;
            };

            float4 RenderText(float4 color, float2 uv, float2 itemSize, uint bold, float4 uvBounds)
            {
                uint w; uint h;
                _FontAtlas.GetDimensions(w, h);
                float2 origSize = float2(uvBounds.zw - uvBounds.xy) * float2(w, h);

                float numerator = max(itemSize.x, itemSize.y) * 0.5;
                float denominator = max(origSize.x, origSize.y) * 0.5;

                const float screenPxRange = numerator / denominator * 3.0; // * 3.0
                float sdf = _FontAtlas.SampleLevel(_FontAtlasSampler, uv, 0.0).r;
                float screenPxDistance = screenPxRange * (sdf - 0.5);
                float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
                return lerp(float4(color.rgb, 0), color, opacity);
            }

            float4 RenderCircle(in GeometryInfo info, float2 p)
            {
                float itemSize = min(info.itemSize.x, info.itemSize.y);
                float sdf = SDFCircle(p, itemSize / 2.0);
                const float invSdf = -sdf;
                float borderThickness = info.borderThickness;

                float4 color = info.fillColor;

                float borderMask = 0.0;
                if (sdf > -borderThickness && sdf <= 0)
		            borderMask = 1.0;

                const float borderSmoothStart = -borderThickness - 1.0;
                const float borderSmoothEnd = -borderThickness;
                borderMask = lerp(borderMask, 1, clamp((sdf - borderSmoothStart) / (borderSmoothEnd - borderSmoothStart), 0, 1));

                borderMask = clamp(borderMask, 0, 1);
                color = lerp(color, info.outlineColor, borderMask); // * clamp(invSdf, 0, 1)

                return lerp(float4(color.rgb, 0), color, -sdf);
            }

            float4 RenderRect(in GeometryInfo info, float2 p)
            {
                const float sdf = SDFBox(p, info.itemSize * 0.5);
                const float invSdf = -sdf;

                float borderThickness = info.borderThickness;

                float4 color = info.fillColor;

                float borderMask = 0.0;
                
                if (sdf > -borderThickness && sdf <= 0)
		            borderMask = 1.0;
                if (info.outlineColor.a < 0.001)
                    info.outlineColor = color;

                if (info.gradientStartIdx < info.gradientEndIdx)
                {
                    float2 pos = info.uv * info.itemSize;
                    float angle = info.gradientRadians;
                    float ratio = (pos.x / info.itemSize.x) * cos(angle) + (pos.y / info.itemSize.y) * sin(angle);
                    if (ratio < 0.0)
                        ratio = 1.0 + ratio;
                    ratio = clamp(ratio, 0.0, 1.0);

                    for (int i = info.gradientStartIdx; i < info.gradientEndIdx; i++)
                    {
                        if (_GradientKeys[i].position <= ratio && ratio <= _GradientKeys[i + 1].position)
                        {
                            color = lerp(_GradientKeys[i].color, _GradientKeys[i + 1].color, 
                                clamp((ratio - _GradientKeys[i].position) / (_GradientKeys[i + 1].position - _GradientKeys[i].position), 0, 1));
                            break;
                        }
                    }
                }

                color = lerp(color, info.outlineColor, borderMask);

                const float sharpness = 50.0;
                return lerp(float4(color.rgb, 0), color, clamp(-sdf * sharpness, 0, 1));
            }

            float4 RenderRoundedRect(in GeometryInfo info, float2 p)
            {
                const float borderThickness = max(info.borderThickness, 0.0);

                float fillSdf = SDFRoundRect(p, info.itemSize * 0.5, float4(info.cornerRadius.z, info.cornerRadius.y, info.cornerRadius.w, info.cornerRadius.x));
                float borderSdf = abs(fillSdf + borderThickness) - borderThickness;
                const float sharpness = 1.0;

                fillSdf = clamp(-fillSdf * sharpness, 0.0, 1.0);
                borderSdf = clamp(-borderSdf * sharpness * 5.0, 0.0, 1.0);

                float4 fillColor = info.fillColor;

                if (info.gradientStartIdx < info.gradientEndIdx)
                {
                    float2 pos = info.uv * info.itemSize;
                    float angle = info.gradientRadians;
                    float ratio = (pos.x / info.itemSize.x) * cos(angle) + (pos.y / info.itemSize.y) * sin(angle);
                    if (ratio < 0.0)
                        ratio = 1.0 + ratio;
                    ratio = clamp(ratio, 0.0, 1.0);

                    for (int i = info.gradientStartIdx; i < info.gradientEndIdx; i++)
                    {
                        if (_GradientKeys[i].position <= ratio && ratio <= _GradientKeys[i + 1].position)
                        {
                            fillColor = lerp(_GradientKeys[i].color, _GradientKeys[i + 1].color, 
                                clamp((ratio - _GradientKeys[i].position) / (_GradientKeys[i + 1].position - _GradientKeys[i].position), 0, 1));
                            break;
                        }
                    }
                }

                float4 color = lerp(fillColor, info.outlineColor, borderSdf);

                return lerp(float4(color.rgb, 0), color, fillSdf);
            }

            float4 RenderRoundedX(in GeometryInfo info, float2 p)
            {
                const float sdf = SDFRoundedX(p, min(info.itemSize.x, info.itemSize.y), info.borderThickness * 0.7);
                const float invSdf = -sdf;

                float4 color = info.fillColor;

                return lerp(float4(color.rgb, 0), color, clamp(invSdf * 5.0, 0, 1));
            }

            float4 RenderTriangle(in GeometryInfo info, float2 p)
            {
                const float sdf = SDFIsoscelesTriangle(p, min(info.itemSize.x, info.itemSize.y));
                const float invSdf = -sdf;
                const float4 color = info.fillColor;

                return lerp(float4(color.rgb, 0), color, invSdf);
            }

            float4 RenderDashedLine(in GeometryInfo info, float2 p)
            {
                const float sdf = SDFDashedLine(p, 
                    float2(-info.itemSize.x, 0.0), 
                    float2(info.itemSize.x, 0.0), 
                    info.dashLength * 0.5 / info.itemSize.x,
                    info.borderThickness);
                
                const float4 color = info.outlineColor;

                return lerp(float4(color.rgb, 0), color, -sdf * 5.0);
            }

            #define idx input.instanceId

            float4 FragMain(PSInput input) : SV_TARGET
            {
                GeometryInfo info;
                info.fillColor = _DrawList[idx].fillColor;
                info.outlineColor = _DrawList[idx].outlineColor;
                info.borderThickness = _DrawList[idx].borderThickness;
                info.itemSize = _DrawList[idx].itemSize;
                info.cornerRadius = _DrawList[idx].cornerRadius;
                info.uv = input.uv;
                info.dashLength = _DrawList[idx].dashLength;
                info.textureIndex = _DrawList[idx].textureIndex;
                info.samplerIndex = _DrawList[idx].samplerIndex;

                float2 uv = input.uv;
                const float2 screenPos = input.screenPosition;
                const float2 scaling = info.outlineColor.xy;
                const float2 tiling = info.outlineColor.zw;
                const uint samplerIndex = _DrawList[idx].samplerIndex;

                float2 p = (uv - float2(0.5, 0.5)) * info.itemSize;

                const float4 clipRect = _ClipRects[_DrawList[idx].clipRect].rect;
                const float4 clipRectCorners = _ClipRects[_DrawList[idx].clipRect].cornerRadius;
                const float2 clipSize = float2(clipRect.z - clipRect.x, clipRect.w - clipRect.y);

                float clipRectSdf = SDFRoundRect(screenPos - clipSize * 0.5 - clipRect.xy, clipSize * 0.5, 
                    float4(clipRectCorners.z, clipRectCorners.y, clipRectCorners.w, clipRectCorners.x));

                if (clipRectSdf > 0.025)
                    discard;

                float4 col = float4(0, 0, 0, 0);
                
                switch (_DrawList[idx].drawType)
                {
                case DRAW_Text:
                    return RenderText(_DrawList[idx].fillColor, input.uv, info.itemSize, _DrawList[idx].bold, input.uvBounds);
                case DRAW_Circle:
                    col = RenderCircle(info, p);
                    break;
                case DRAW_Rect:
                    col = RenderRect(info, p);
                    break;
                case DRAW_RoundedRect:
                    col = RenderRoundedRect(info, p);
                    break;
                case DRAW_RoundedX:
                    col = RenderRoundedX(info, p);
                    break;
                case DRAW_Texture:
                    col = _Textures[_DrawList[idx].textureIndex].SampleLevel(_TextureSamplers[samplerIndex], uv * scaling - tiling, 0.0) * float4(info.fillColor.rgb, 1.0);
                    break;
                case DRAW_FrameBuffer:
                    col = _Textures[_DrawList[idx].textureIndex + _FrameIndex].SampleLevel(_TextureSamplers[samplerIndex], uv * scaling - tiling, 0.0) * float4(info.fillColor.rgb, 1.0);
                    break;
                case DRAW_Triangle:
                    col = RenderTriangle(info, p);
                    break;
                case DRAW_DashedLine:
                    col = RenderDashedLine(info, p);
                    break;
                }

                float t = clamp(-clipRectSdf, 0, 1);

                return lerp(float4(col.rgb, 0), col, t);
            }

            #endif

            ENDHLSL
        }
    }
}