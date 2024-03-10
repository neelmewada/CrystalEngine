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
            #include "Core/SceneData.hlsli"
            #include "Core/ViewData.hlsli"
            #include "Core/ObjectData.hlsli"

            #include "Core/Gamma.hlsli"

            struct VSInput
            {
                float3 position : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct PSInput
            {
                float4 position : SV_POSITION;
                float2 uv : TEXCOORD1;
            };

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                //float3 worldPos = LOCAL_TO_WORLD_SPACE(float4(input.position, 1.0), input).xyz;
                //o.position = WORLD_TO_CLIP_SPACE(float4(worldPos, 1.0));
                o.position = float4(input.position, 1.0);
                o.uv = input.uv;
                return o;
            }
            #endif

            #if FRAGMENT

            Texture2D<float> _FontAtlas : SRG_PerMaterial(t0);
            SamplerState _FontAtlasSampler : SRG_PerMaterial(s1);

            float4 FragMain(PSInput input) : SV_TARGET
            {
                float a = _FontAtlas.Sample(_FontAtlasSampler, input.uv).r;
                if (a > 0.5) {
                    a = 1.0;
                } else {
                    a = 0.0;
                }
                return float4(a, a, a, a);
            }

            #endif

            ENDHLSL
        }
    }
}