Shader "Test Shader"
{
    Properties 
    {
        _Material.albedo("Albedo", Color) = (0, 0, 0, 0)
        
    }
    SubShader
    {
        LOD 100

        Pass
        {
            Name "Main"
            Tags {  }

            HLSLPROGRAM

            #include "Macros.hlsl"

            #pragma vertex VertMain
            #pragma fragment FragMain

            struct VertexInfo
            {
                float3 position : POSITION;
                float3 normal : NORMAL;
                float2 uv0 : TEXCOORD0;
            };

            struct CameraData
            {
                float4x4 viewMatrix;
                float4x4 projectionMatrix;
                float4x4 viewProjectionMatrix;
            };

            struct ModelData
            {
                float4x4 modelMatrix;
            };

            struct MaterialData
            {
                float4 albedo;
                float roughness;
                float normalStrength;
                float metallic;
            };
            
            ConstantBuffer<CameraData> _Camera : SRG_PerView(b);

            ConstantBuffer<ModelData> _ModelData : SRG_PerObject(b);

            ConstantBuffer<MaterialData> _Material : SRG_PerMaterial(b);
            Texture2D _AlbedoTex : SRG_PerMaterial(t);
            Texture2D _NormalMap : SRG_PerMaterial(t);
            Texture2D _Metallic : SRG_PerMaterial(t);

            void VertMain(VertexInfo input)
            {
                
            }

            void FragMain()
            {

            }

            ENDHLSL
        }
    }
}
