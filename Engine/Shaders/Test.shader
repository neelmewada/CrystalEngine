Shader "Test Shader"
{
    Properties 
    {
        _Material ("Material") = {
            albedo ("Albedo", Vector) = 0
            albedoTex ("Albedo Texture", 2D) = "black"
            roughness ("Roughness", Float) = 0
        }
    }

    SubShader
    {
        LOD 100

        Pass
        {
            Tags { "Vertex" = "VertMain", "Fragment" = "FragMain" }

            HLSLPROGRAM

            #include "Macros.hlsl"
            
            struct PerViewData
            {
                float4x4 viewMatrix;
                float4x4 viewProjectionMatrix;
            };

            struct ModelData
            {
                float4x4 modelMatrix;
            };

            struct MaterialData
            {
                float4 albedo;
                Texture2D albedoTex;
                float roughness;
            };
            
            ConstantBuffer<PerViewData> _PerView : SRG_PerView(b);

            ConstantBuffer<ModelData> _ModelData : SRG_PerObject(b);

            ConstantBuffer<MaterialData> _Material : SRG_PerMaterial(b);

            void VertMain()
            {

            }

            ENDHLSL
        }
    }
}
