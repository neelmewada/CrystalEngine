Shader "UI/Text"
{
    Properties
    {
        _FgColor ("Color", Color) = (1, 1, 1, 1)
        _BgColor ("BG Color", Color) = (0, 0, 0, 0,)
    }

    SubShader
    {
        Tags {
            "Blend" = "SrcAlpha,OneMinusSrcAlpha"
        }

        Pass
        {
            Name "Text"
            Tags { 
                "Vertex"="VertMain", "Fragment"="FragMain", "DrawListTag"="ui"
            }
            ZWrite Off
            ZTest Off
            Cull Off

            HLSLPROGRAM
            
            #include "Core/Macros.hlsli"
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
                float bgMask : TEXCOORD1;
            };

            struct DrawItem
            {
                float4x4 transform;
                float4 atlasUV;
                float bgMask;
            };

            StructuredBuffer<DrawItem> _DrawList : SRG_PerDraw(t0);

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                float4 uvBounds = _DrawList[input.instanceId].atlasUV;
                o.position = mul(float4(input.position, 1.0), _DrawList[input.instanceId].transform);
                o.uv = float2(lerp(uvBounds.x, uvBounds.z, input.uv.x), lerp(uvBounds.y, uvBounds.w, input.uv.y));
                o.bgMask = _DrawList[input.instanceId].bgMask;
                return o;
            }
            #endif

            #if FRAGMENT

            Texture2D<float> _FontAtlas : SRG_PerMaterial(t0);
            SamplerState _FontAtlasSampler : SRG_PerMaterial(s1);

            cbuffer _MaterialData : SRG_PerMaterial(b2)
            {
                float4 _FgColor;
                float4 _BgColor;
                uint _Bold;
            };

            float4 FragMain(PSInput input) : SV_TARGET
            {
                uint w; uint h;
                _FontAtlas.GetDimensions(w, h);

                const int spread = 1;

                float a = 0;

                /*for (int x = -spread; x <= spread; x++)
                {
                    for (int y = -spread; y <= spread; y++)
                    {
                        a += _FontAtlas.SampleLevel(_FontAtlasSampler, input.uv + float2(float(x) / w, float(y) / h), 0.0).r / float(spread * 9);
                    }
                }*/

                a = _FontAtlas.SampleLevel(_FontAtlasSampler, input.uv, 0.0).r;

                float boldValue = (1.0 - clamp(float(_Bold), 0, 1)) * 0.1;

                float upperMidPoint = 0.5 + boldValue;
                float lowerMidPoint = 0.4 + boldValue;
                float t = (a - lowerMidPoint) / (upperMidPoint - lowerMidPoint);

                a = lerp(0, 1, clamp(t, 0, 1));
                
                float4 foreground = float4(_FgColor.rgb, a * _FgColor.a);

                return lerp(foreground, _BgColor, input.bgMask);
            }

            #endif

            ENDHLSL
        }
    }
}