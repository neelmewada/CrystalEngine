Shader "Utils/Texture Generator"
{
    Properties
    {
        _InputTexture ("Input Texture", Tex2D) = "white"
    }

    SubShader
    {
        Tags {
            "Blend" = "One,Zero"
        }

        Pass
        {
            Name "ColorPickerGen"
            Tags { 
                "Vertex"="VertMain", "Fragment"="FragMain"
            }
            ZWrite Off
            ZTest Off
            Cull Off

            HLSLPROGRAM

            #include "Core/Macros.hlsli"
            #include "Core/Utils.hlsli"

            struct VSInput
            {
                float3 position : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct PSInput
            {
                float4 position : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                o.position = float4(input.position, 1.0);
                o.uv = input.uv;
                return o;
            }

            #endif // VERTEX

            #if FRAGMENT

            float3 ToRGB(float3 hsv)
            {
                float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                float3 p = abs(frac(hsv.xxx + K.xyz) * 6.0 - K.www);

                return hsv.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), hsv.y);
            }

            float4 FragMain(PSInput input) : SV_Target
            {
                float2 p = input.uv;

                float h = p.x;
                float s = clamp(p.y / 0.5, 0.0, 1.0);
                float v = 1.0 - clamp((p.y - 0.5) / 0.5, 0.0, 1.0);

                return float4(ToRGB(float3(h, s, v)), 1);
            }

            #endif // FRAGMENT

            ENDHLSL
        }
    }
}