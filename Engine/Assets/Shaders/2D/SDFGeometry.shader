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
            };

            struct DrawItem
            {
                float4x4 transform;
                float4 fillColor;
                float4 outlineColor;
                float4 cornerRadius;
                float2 itemSize; // item size in pixels
                float borderThickness;
                uint drawType; // enum DrawType;
                uint charIndex; // For character drawing
                uint bold;
                uint clipRect;
                uint textureIndex;
            };

            StructuredBuffer<DrawItem> _DrawList : SRG_PerDraw(t0);

            StructuredBuffer<float4> _ClipRects : SRG_PerDraw(t1);

            SamplerState _TextureSampler : SRG_PerDraw(s2);

            #define MAX_TEXTURES 10000

            Texture2D<float4> _Textures[MAX_TEXTURES] : SRG_PerDraw(t3);

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

            struct GeometryInfo
            {
                float4 fillColor;
                float4 outlineColor;
                float4 cornerRadius;
                float2 itemSize;
                float2 uv;
                float borderThickness;
            };

            float4 RenderText(float4 color, float2 uv, float2 itemSize, uint bold)
            {
                const float screenPxRange = itemSize.x / 7.5;// 7.5
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

                color = lerp(color, info.outlineColor, borderMask);

                return lerp(float4(color.rgb, 0), color, clamp(-sdf * 5.0, 0, 1)); // 1.5 = sharpness
            }

            float4 RenderRoundedRect(in GeometryInfo info, float2 p)
            {
                const float sdf = SDFRoundRect(p, info.itemSize * 0.5, float4(info.cornerRadius.z, info.cornerRadius.y, info.cornerRadius.w, info.cornerRadius.x));
                const float invSdf = -sdf;

                const float borderThickness = info.borderThickness;

                float4 color = info.fillColor;

                float borderMask = 0.0;
                //if (sdf > -borderThickness && sdf <= 0)
		        //    borderMask = 1.0;

                if (borderThickness > 0.1)
                {
                    const float borderSmoothStart = -borderThickness - 1.0;
                    const float borderSmoothEnd = -borderThickness;
                    borderMask = lerp(borderMask, 1, clamp((sdf - borderSmoothStart) / (borderSmoothEnd - borderSmoothStart), 0, 1));
                    borderMask = clamp(borderMask, 0, 1);
                }

                color = lerp(color, info.outlineColor, borderMask);

                return lerp(float4(color.rgb, 0), color, invSdf * 0.9);
            }

            float4 RenderRoundedX(in GeometryInfo info, float2 p)
            {
                const float sdf = SDFRoundedX(p, min(info.itemSize.x, info.itemSize.y), info.borderThickness * 0.7);
                const float invSdf = -sdf;

                float4 color = info.fillColor;

                return lerp(float4(color.rgb, 0), color, clamp(invSdf * 5.0, 0, 1));
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
                float2 uv = input.uv;
                const float2 screenPos = input.screenPosition;

                float2 p = (uv - float2(0.5, 0.5)) * info.itemSize;

                const float4 clipRect = _ClipRects[_DrawList[idx].clipRect];
                if (screenPos.x < clipRect.x || screenPos.x > clipRect.z || screenPos.y < clipRect.y || screenPos.y > clipRect.w)
                    discard;
                
                switch (_DrawList[idx].drawType)
                {
                case DRAW_Text:
                    return RenderText(_DrawList[idx].fillColor, input.uv, info.itemSize, _DrawList[idx].bold);
                case DRAW_Circle:
                    return RenderCircle(info, p);
                case DRAW_Rect:
                    return RenderRect(info, p);
                case DRAW_RoundedRect:
                    return RenderRoundedRect(info, p);
                case DRAW_RoundedX:
                    return RenderRoundedX(info, p);
                case DRAW_Texture:
                    return _Textures[_DrawList[idx].textureIndex].SampleLevel(_TextureSampler, uv, 0.0);
                }

                return float4(0, 0, 0, 0);
            }

            #endif

            ENDHLSL
        }
    }
}