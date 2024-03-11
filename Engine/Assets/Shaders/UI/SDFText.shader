Shader "UI/SDF Text"
{
    Properties
    {
        _FgColor ("Color", Color) = (1, 1, 1, 1)
        _BgColor ("BG Color", Color) = (0, 0, 0, 0)
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
                float2 fullUV : TEXCOORD2;
            };

            cbuffer _PerDraw : SRG_PerDraw(b0)
            {
                uint _ScreenWidth;
                uint _ScreenHeight;
            };

            struct DrawItem
            {
                float4x4 transform;
                float4 atlasUV;
            };

            StructuredBuffer<DrawItem> _DrawList : SRG_PerDraw(t1);

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                float4 uvBounds = _DrawList[input.instanceId].atlasUV;
                o.position = mul(float4(input.position, 1.0), _DrawList[input.instanceId].transform);
                o.uv = float2(lerp(uvBounds.x, uvBounds.z, input.uv.x), lerp(1 - uvBounds.y, 1 - uvBounds.w, 1 - input.uv.y));
                o.fullUV = input.uv;
                return o;
            }
            #endif

            #if FRAGMENT

            Texture2D<float4> _FontAtlas : SRG_PerMaterial(t0);
            SamplerState _FontAtlasSampler : SRG_PerMaterial(s1);

            cbuffer _MaterialData : SRG_PerMaterial(b3)
            {
                float4 _FgColor;
                float4 _BgColor;
                float _PixRange;
            };

            inline float median(float r, float g, float b) 
            {
                return max(min(r, g), min(max(r, g), b));
            }

            float screenPxRange(float2 texCoord) 
            {
                uint w; uint h;
                _FontAtlas.GetDimensions(w, h);

                float2 unitRange = float2(_PixRange, _PixRange) / float2(w, h);
                float2 screenTexSize = float2(1.0, 1.0) / abs(ddx(texCoord) + ddy(texCoord));
                return max(0.5 * dot(unitRange, screenTexSize), 1.0);
            }

            float4 FragMain(PSInput input) : SV_TARGET
            {
                float4 msdf = _FontAtlas.SampleLevel(_FontAtlasSampler, input.uv, 0.0);
                float sd = median(msdf.r, msdf.g, msdf.b);
                float screenPxDistance = screenPxRange(input.uv) * (sd - 0.5);
                float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
                return lerp(float4(0, 0, 0, 0), float4(1, 1, 1, 1), opacity);
            }

            #endif

            ENDHLSL
        }
    }
}