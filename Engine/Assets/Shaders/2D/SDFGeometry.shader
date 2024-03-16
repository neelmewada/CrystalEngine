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
            Name "Rect"
            Tags { 
                "Vertex"="VertMain", "Fragment"="FragMain"
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
            };

            struct DrawItem
            {
                float4x4 transform;
                float4 fillColor;
                float4 outlineColor;
                float2 itemSize; // item size in pixels
                float borderThickness;
                uint drawType; // 0 = None ; 1 = Text ; 2 = Circle ; 3 = Rect ; 4 = Rounded Rect
                uint charIndex; // For character drawing
                uint bold;
            };

            StructuredBuffer<DrawItem> _DrawList : SRG_PerDraw(t0);

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

            float4 RenderText(float4 color, float2 uv, uint bold)
            {
                float a = _FontAtlas.SampleLevel(_FontAtlasSampler, uv, 0.0).r;

                float boldValue = (1.0 - clamp(float(bold), 0, 1)) * 0.1;

                float upperMidPoint = 0.5 + boldValue; // small size: 0.42
                float lowerMidPoint = 0.4 + boldValue; // small size: 0.35
                float t = (a - lowerMidPoint) / (upperMidPoint - lowerMidPoint);

                a = lerp(0, 1, clamp(t, 0, 1));
                
                return float4(color.rgb, a * color.a);
            }

            inline float SDFCircle(float2 uv)
            {
                return length(uv - float2(0.5, 0.5)) - 0.5;
            }

            struct GeometryInfo
            {
                float4 fillColor;
                float4 outlineColor;
                float2 itemSize;
                float borderThickness;
            };

            float4 RenderCircle(in GeometryInfo info, float2 uv)
            {
                float itemSize = (info.itemSize.x + info.itemSize.y) / 2.0;
                float sdf = SDFCircle(uv);
                const float invSdf = -sdf;
                float borderThickness = info.borderThickness / itemSize;

                float4 color = lerp(float4(info.fillColor.rgb, 0), info.fillColor, -sdf * _SDFSmoothness);
                color = clamp(color, float4(0, 0, 0, 0), float4(1, 1, 1, 1));

                float borderMask = 0.0;
                if (sdf > -borderThickness && sdf <= 0)
		            borderMask = 1.0;

                borderMask = clamp(borderMask, 0, 1);
                color = lerp(color, info.outlineColor, borderMask * clamp(invSdf * _SDFSmoothness * 0.9, 0, 1));
                return color;
            }

            float4 RenderRect(in GeometryInfo info, float2 uv)
            {
                float4 color = info.fillColor;
                float2 borderThickness = float2(info.borderThickness, info.borderThickness) / info.itemSize;

                float borderMask = 0.0;
                if (uv.x < borderThickness.x || uv.x > 1.0 - borderThickness.x)
                    borderMask = 1.0;
                if (uv.y < borderThickness.y || uv.y > 1.0 - borderThickness.y)
                    borderMask = 1.0;

                borderMask = clamp(borderMask, 0, 1);
                color = lerp(color, info.outlineColor, borderMask);
                return color;
            }

            #define idx input.instanceId

            float4 FragMain(PSInput input) : SV_TARGET
            {
                GeometryInfo info;
                info.fillColor = _DrawList[idx].fillColor;
                info.outlineColor = _DrawList[idx].outlineColor;
                info.borderThickness = _DrawList[idx].borderThickness;
                info.itemSize = _DrawList[idx].itemSize;

                if (_DrawList[idx].drawType == DRAW_Text) // Text
                {
                    return RenderText(_DrawList[idx].fillColor, input.uv, _DrawList[idx].bold);
                }
                else if (_DrawList[idx].drawType == DRAW_Circle) // Circle
                {
                    return RenderCircle(info, input.uv);
                }
                else if (_DrawList[idx].drawType == DRAW_Rect) // Rectangle
                {
                    return RenderRect(info, input.uv);
                }
                else if (_DrawList[idx].drawType == DRAW_RoundedRect) // Rounded Rectangle
                {

                }

                return float4(0, 0, 0, 0);
            }

            #endif

            ENDHLSL
        }
    }
}