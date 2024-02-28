Shader "CubeMap/IBL"
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
            Name "Grayscale"
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

            float4 FragMain(PSInput input) : SV_TARGET
            {
                float3 texColor = _InputTexture.Sample(_InputSampler, input.uv).rgb;
                float gray = dot(float3(0.2126, 0.7152, 0.0722), texColor);
                if (isnan(gray))
                    gray = 65000;
                return float4(gray, gray, gray, 1.0);
            }
            #endif

            ENDHLSL
        }

        Pass
        {
            Name "RowAverage"
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

            Texture2D<float> _InputTexture : SRG_PerMaterial(t0);
            SamplerState _InputSampler : SRG_PerMaterial(s1);

            float p2t(in float p, in int noOfPixels) {
                return (p + 0.5) / float(noOfPixels);
            }

            float t2p(in float t, in int noOfPixels) {
                return t * float(noOfPixels) - 0.5; 
            }

            int round2Integer(in float val) {
                return int(val + 0.5);
            }

            float4 FragMain(PSInput input) : SV_TARGET
            {
                uint inputImageWidth;
                uint inputImageHeight;
                _InputTexture.GetDimensions(inputImageWidth, inputImageHeight);

                float sum = 0.0;
                int y = round2Integer(t2p(input.uv.y, inputImageHeight));
                for(int x = 0; x < inputImageWidth; x++) {
                    float gray = _InputTexture[int2(x, y)].r;
                    if (isnan(gray))
                        gray = 0;
                    sum += gray;
                }
                sum /= float(inputImageWidth);

                return float4(sum, sum, sum, 1.0);
            }
            #endif

            ENDHLSL
        }

        Pass
        {
            Name "ColumnAverage"
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

            Texture2D<float> _InputTexture : SRG_PerMaterial(t0);
            SamplerState _InputSampler : SRG_PerMaterial(s1);

            float p2t(in float p, in int noOfPixels) {
            return (p + 0.5) / float(noOfPixels);
            }

            float t2p(in float t, in int noOfPixels) {
            return t * float(noOfPixels) - 0.5;
            }

            int round2Integer(in float val) {
            return int(val + 0.5);
            }

            float4 FragMain(PSInput input) : SV_TARGET
            {
                uint inputImageWidth;
                uint inputImageHeight;
                _InputTexture.GetDimensions(inputImageWidth, inputImageHeight);

                float sum = 0.0;
                int x = round2Integer(t2p(input.uv.x, inputImageWidth));

                for(int y = 0; y < inputImageHeight; y++) {
                    float gray = _InputTexture[int2(x, y)].r;
                    if (isnan(gray))
                        gray = 0;
                    sum += gray;
                }
                sum /= float(inputImageHeight);

                return float4(sum, sum, sum, 1.0);
            }
            #endif


            ENDHLSL
        }

        Pass
        {
            Name "Division"
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

            Texture2D<float> _InputA : SRG_PerMaterial(t0);
            Texture2D<float> _InputB : SRG_PerMaterial(t1);
            SamplerState _InputSampler : SRG_PerMaterial(s2);

            float p2t(in float p, in int noOfPixels) {
            return (p + 0.5) / float(noOfPixels);
            }

            float t2p(in float t, in int noOfPixels) {
            return t * float(noOfPixels) - 0.5;
            }

            int round2Integer(in float val) {
            return int(val + 0.5);
            }

            float4 FragMain(PSInput input) : SV_TARGET
            {
                float a = _InputA.Sample(_InputSampler, input.uv).r;
                float b = _InputB.Sample(_InputSampler, input.uv).r;
                float c = 0.0;
                if(abs(b) > 1e-8) { // prevent division by zero
                    c = a / b;
                }
                return float4(c, c, c, 1.0);
            }
            #endif

            ENDHLSL
        }

        Pass
        {
            Name "CDFMarginalInverse"
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

            Texture2D<float> _InputTexture : SRG_PerMaterial(t0);
            SamplerState _InputSampler : SRG_PerMaterial(s1);

            float p2t(in float p, in int noOfPixels) {
                return (p + 0.5) / float(noOfPixels);
            }

            float t2p(in float t, in int noOfPixels) {
                return t * float(noOfPixels) - 0.5;
            }

            int round2Integer(in float val) {
                return int(val + 0.5);
            }

            float4 FragMain(PSInput input) : SV_TARGET
            {
                uint inputImageWidth;
                uint inputImageHeight;
                _InputTexture.GetDimensions(inputImageWidth, inputImageHeight);

                float sum = 0.0;
                int x = round2Integer(t2p(input.uv.x, inputImageWidth));
                float u = 1.0 - saturate(input.uv.y); // Invert the vertical UV coord
                int y;

                for(y = 0; y < inputImageHeight; y++) {
                    float gray = _InputTexture[int2(x, y)].r;
                    sum += gray / float(inputImageHeight);
                    if(sum >= u) {
                        break;
                    }
                }
                float cdfYInv = p2t(float(y), inputImageHeight);

                return float4(cdfYInv, cdfYInv, cdfYInv, 1.0);
            }
            #endif

            ENDHLSL
        }

        Pass
        {
            Name "CDFConditionalInverse"
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

            Texture2D<float> _InputTexture : SRG_PerMaterial(t0);
            SamplerState _InputSampler : SRG_PerMaterial(s1);

            float p2t(in float p, in int noOfPixels) {
                return (p + 0.5) / float(noOfPixels);
            }

            float t2p(in float t, in int noOfPixels) {
                return t * float(noOfPixels) - 0.5;
            }

            int round2Integer(in float val) {
                return int(val + 0.5);
            }

            float4 FragMain(PSInput input) : SV_TARGET
            {
                uint inputImageWidth;
                uint inputImageHeight;
                _InputTexture.GetDimensions(inputImageWidth, inputImageHeight);

                float sum = 0.0;
                int y = round2Integer(t2p(input.uv.y, inputImageHeight));
                float u = input.uv.x;
                int x;
                for(x = 0; x < inputImageWidth; x++) {
                    float gray = _InputTexture[int2(x, y)].r;
                    sum += gray / float(inputImageWidth);
                    if(sum >= u) {
                        break;
                    }
                }
                float cdfXInv = p2t(float(x), inputImageWidth);

                return float4(cdfXInv, cdfXInv, cdfXInv, 1.0);
            }
            #endif

            ENDHLSL
        }
    }
}