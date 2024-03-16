Shader "Utils/MipMap Generator"
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
            Name "Diffuse"
            Tags { 
                "Vertex"="VertMain", "Fragment"="FragMain"
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
            #endif

            #if FRAGMENT

            Texture2D<float4> _InputTexture : SRG_PerMaterial(t0);
            SamplerState _InputSampler : SRG_PerMaterial(s1);

            float4 FragMain(PSInput input) : SV_Target
            {
                uint width;
                uint height;
                _InputTexture.GetDimensions(width, height);

                const float weights[9] = {
                    1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
                    1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
                    1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f
                };

                float2 texCoord = input.uv;
                float2 inputCoords = texCoord * float2(width, height);

                float4 colorSum = float4(0.0f, 0.0f, 0.0f, 0.0f);

                // Apply box filter to the 3x3 block
                for (int i = -1; i <= 1; ++i)
                {
                    for (int j = -1; j <= 1; ++j)
                    {
                        // Sample the color from the input image using linear filtering
                        float4 color = _InputTexture.Sample(_InputSampler, (inputCoords + float2(i, j)) / float2(width, height));
                        // Multiply color by weight and accumulate
                        colorSum += color * weights[(i + 1) * 3 + (j + 1)];
                    }
                }
                
                // Return the averaged color
                return colorSum;
            }

            #endif

            ENDHLSL
        }
    }
}