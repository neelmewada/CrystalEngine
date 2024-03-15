Shader "2D/SDF Geometry"
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
            };

            struct SDFDrawItem
            {
                float4 bounds;
            };

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                o.position = mul(input.position, viewProjectionMatrix);
                o.uv = input.uv;
                return o;
            }

            #endif

            #if FRAGMENT

            float4 FragMain(PSInput input) : SV_TARGET
            {
                return float4(1, 1, 1, 1);
            }

            #endif

            ENDHLSL
        }
    }
}