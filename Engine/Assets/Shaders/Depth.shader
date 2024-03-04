Shader "Depth"
{
    Properties
    {
        
    }

    SubShader
    {

        Pass
        {
            Name "Depth"
            Tags { 
                "Vertex"="VertMain", "Fragment"="FragMain", "DrawListTag"="depth"
            }
            ZWrite On
            ZTest LEqual

            HLSLPROGRAM
            
            #include "Core/Macros.hlsli"
            #include "Core/SceneData.hlsli"
            #include "Core/ViewData.hlsli"
            #include "Core/ObjectData.hlsli"

            #include "Core/Gamma.hlsli"

            struct VSInput
            {
                float3 position : POSITION;
                INSTANCING()
            };

            struct PSInput
            {
                float4 position : SV_POSITION;
            };

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput o;
                o.position = LOCAL_TO_CLIP_SPACE(float4(input.position, 1.0), input);
                return o;
            }
            #endif

            #if FRAGMENT

            float4 FragMain(PSInput input) : SV_TARGET
            {
                return float4(0, 0, 0, 1);
            }

            #endif

            ENDHLSL
        }
    }
}