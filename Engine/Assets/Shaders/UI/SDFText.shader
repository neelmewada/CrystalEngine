Shader "UI/SDF Text"
{
    Properties
    {
        
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
                float2 uv : TEXCOORD1;
                uint instanceId : SV_INSTANCEID;
            };

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                o.position = float4(input.position, 1.0);
                o.uv = input.uv;
                o.instanceId = input.instanceId;
                return o;
            }
            #endif

            #if FRAGMENT

            Texture2D<float4> _FontAtlas : SRG_PerMaterial(t0);
            SamplerState _FontAtlasSampler : SRG_PerMaterial(s1);

            cbuffer _MaterialData : SRG_PerMaterial(b3)
            {
                float4 _PixRange;
            };

            cbuffer _PerDraw : SRG_PerDraw(b0)
            {
                uint2 _ScreenSize;
            };

            float4 FragMain(PSInput input) : SV_TARGET
            {
                float a = _FontAtlas.Sample(_FontAtlasSampler, input.uv).r;
                if (a > 0.5) {
                    a = 1.0;
                } else {
                    a = min(0.0, input.instanceId);
                }
                return float4(a, a, a, a);
            }

            #endif

            ENDHLSL
        }
    }
}