Shader "Test Shader"
{
    Properties
    {
        _Albedo ("Albedo", Color) = (1, 1, 1, 1)
        _Metallic ("Metallic", Float) = 1.0
        _Roughness ("Roughness", Float) = 1.0
        _NormalStrength ("Normal Strength", Float) = 1.0
        _AmbientOcclusion ("Ambient Occlusion", Float) = 1.0

        _AlbedoTex ("Albedo Map", Tex2D) = "white"
        _RoughnessTex ("Roughness Map", Tex2D) = "white"
        _NormalMap ("Normal Map", Tex2D) = "bump"
        _MetallicTex ("Metallic Map", Tex2D) = "white"
    }

    SubShader
    {
        Tags {
            "Blend" = "SrcAlpha,OneMinusSrcAlpha"
        }

        Pass
        {
            Name "Opaque"
            Tags { "Vertex"="VertMain", "Fragment"="FragMain", "DrawListTag"="opaque" }
            ZWrite Off
            ZTest LEqual
            Cull Back

            HLSLPROGRAM
            
            #include "Core/Macros.hlsli"
            #include "Core/SceneData.hlsli"
            #include "Core/ViewData.hlsli"
            #include "Core/ObjectData.hlsli"

            #include "Core/Gamma.hlsli"

            struct VSInput
            {
                float3 position : POSITION;
                float3 normal : NORMAL;
                float4 tangent : TANGENT;
                float2 uv : TEXCOORD0;
                INSTANCING()
            };

            struct PSInput
            {
                float4 position : SV_POSITION;
                float3 worldPos : TEXCOORD0;
                float3 normal : TEXCOORD1;
                float2 uv : TEXCOORD2;
                float3 tangent : TEXCOORD3;
                float3 bitangent : TEXCOORD4;
            };

            #if VERTEX

            PSInput VertMain(VSInput input)
            {
                PSInput output;
                output.position = LOCAL_TO_CLIP_SPACE(float4(input.position, 1.0), input);
                output.worldPos = LOCAL_TO_WORLD_SPACE(float4(input.position, 1.0), input).xyz;
                output.normal = LOCAL_TO_WORLD_SPACE(float4(input.normal, 0), input).xyz;
                output.tangent = LOCAL_TO_WORLD_SPACE(float4(input.tangent.xyz, 0), input).xyz;
                output.bitangent = -cross(output.normal, output.tangent);
                output.uv = input.uv;
                return output;
            }
            #endif

            #if FRAGMENT

            cbuffer _Material : SRG_PerMaterial(b0)
            {
                float4 _Albedo;
                float  _Metallic;
                float  _Roughness;
                float  _NormalStrength;
                float  _AmbientOcclusion;
            };

            Texture2D<float4> _AlbedoTex : SRG_PerMaterial(t1);
            Texture2D<float> _RoughnessTex : SRG_PerMaterial(t2);
            Texture2D<float4> _NormalMap : SRG_PerMaterial(t3);
            Texture2D<float> _MetallicTex : SRG_PerMaterial(t4);

            float4 FragMain(PSInput input) : SV_TARGET
            {
                return float4(0, 0, 0, 1);
            }

            #endif

            ENDHLSL
        }
    }
}