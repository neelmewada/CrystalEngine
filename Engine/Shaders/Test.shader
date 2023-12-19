Shader "Test Shader"
{
    Properties 
    {
        _Material("Material", ConstantBuffer)
        {
            [MainColor]
            albedo("Albedo", Color) = (1, 1, 1, 1)
            roughness("Roughness", Float) = 1.0
            normalStrength("Normal Strength", Range(0, 2)) = 1.0
            metallic("Metallic", Float) = 0.0
        }

        _AlbedoTex("Albedo Map", 2D) = "black"
    }
    SubShader
    {
        LOD 100
        Tags {
            "Blend" = "SrcAlpha,OneMinusSrcAlpha",
        }

        Pass
        {
            Name "Main"
            Tags {  }

            HLSLPROGRAM

            #include "ShaderLibrary.hlsli"

            #pragma vertex VertMain
            #pragma fragment FragMain

            struct VertexInfo
            {
                float3 position : POSITION;
                float3 normal : NORMAL;
                float2 uv0 : TEXCOORD0;
            };

            struct v2f
            {
                float4 position : SV_POSITION;
            };

            cbuffer _Material : SRG_PerMaterial(b)
            {
                float4 albedo;
                float roughness;
                float normalStrength;
                float metallic;
            };

            Texture2D _AlbedoTex : SRG_PerMaterial(t);

            v2f VertMain(VertexInfo input)
            {
                v2f o;
                o.position = mul(mul(mul(float4(input.position, 1.0), modelMatrix), viewMatrix), projectionMatrix);
                return o;
            }

            float4 FragMain(v2f input) : SV_TARGET
            {
                return float4(1.0, 0, 1.0, 1.0);
            }

            ENDHLSL
        }
    }
}
