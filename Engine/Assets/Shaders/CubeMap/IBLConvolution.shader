Shader "CubeMap/IBL Convolution"
{
    Properties
    {
        _InputTexture ("Input Texture", Tex2D) = "white"
    }

    SubShader
    {
        Tags {
            "Blend" = "One,Zero",
            "Platform" = "Desktop"
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
            Texture2D<float> pdfJoint : SRG_PerMaterial(t1);
            Texture2D<float> cdfYInv : SRG_PerMaterial(t2);
            Texture2D<float> cdfXInv : SRG_PerMaterial(t3);

            SamplerState _InputSampler : SRG_PerMaterial(s4);

            // convert texture coordinates to pixels
            inline float t2p(in float t, in int noOfPixels) {
                return t * float(noOfPixels) - 0.5;
            }

            // from http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
            // Hacker's Delight, Henry S. Warren, 2001
            float radicalInverse(uint bits) {
                bits = (bits << 16u) | (bits >> 16u);
                bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
                bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
                bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
                bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
                return float(bits) * 2.3283064365386963e-10; // / 0x100000000
            }

            inline float2 hammersley(uint n, uint N) {
                return float2(float(n) / float(N), radicalInverse(n));
            }

            float halton(uint base, uint index) {
                float result = 0.0;
                float digitWeight = 1.0;
                while (index > 0u) {
                    digitWeight = digitWeight / float(base);
                    uint nominator = index % base; // compute the remainder with the modulo operation
                    result += float(nominator) * digitWeight;
                    index = index / base; 
                }
                return result;
            }

            // Hash Functions for GPU Rendering, Jarzynski et al.
            // http://www.jcgt.org/published/0009/03/02/
            float3 random_pcg3d(uint3 v) {
                v = v * 1664525u + 1013904223u;
                v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
                v ^= v >> 16u;
                v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
                return float3(v) * (1.0/float(0xffffffffu));
            }

            float3 SphericalEnvmapToDirection(float2 tex) {
                float theta = PI * (1.0 - tex.y);
                float phi = 2.0 * PI * (0.5 - tex.x);
                return float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            }

            float2 DirectionToSphericalEnvmap(float3 dir) {
                float phi = atan2(dir.y, dir.x);
                float theta = acos(dir.z);
                float s = 0.5 - phi / (2.0 * PI);
                float t = 1.0 - theta / PI;
                return float2(s, t);
            }

            float3x3 GetNormalFrame(in float3 normal) {
                float3 someVec = float3(1.0, 0.0, 0.0);
                float dd = dot(someVec, normal);
                float3 tangent = float3(0.0, 1.0, 0.0);
                if(1.0 - abs(dd) > 1e-6) {
                    tangent = normalize(cross(someVec, normal));
                }
                float3 bitangent = cross(normal, tangent);
                return float3x3(tangent, bitangent, normal);
            }

            float Fill(float sdf) {
                return clamp(0.5 - sdf / fwidth(sdf), 0.0, 1.0);
            }

            float Circle(float2 pos, float2 center, float radius) {
                return length(pos - center) - radius;
            }

            float3 PreFilterEnvMapDiffuse(in float2 tex)
            {
                uint width;
                uint height;
                _InputTexture.GetDimensions(width, height);

                float px = t2p(tex.x, width);
                float py = t2p(tex.y, height);
                float3 normal = SphericalEnvmapToDirection(tex);
                float3 result = float3(0, 0, 0);

                const uint SampleCount = 1024;
                const uint N = SampleCount;

                for(uint n = 1u; n <= N; n++) 
                {
                    float2 random = float2(halton(2u, n), halton(3u, n));
                    float sampleY = cdfYInv.Sample(_InputSampler, float2(0.5, random.y)).r;
                    float sampleX = cdfXInv.Sample(_InputSampler, float2(random.x, sampleY)).r;
                    float2 sampleLocation = float2(sampleX, sampleY);
                    
                    float3 radiance = _InputTexture.Sample(_InputSampler, sampleLocation).rgb;
                    float pdf = pdfJoint.Sample(_InputSampler, sampleLocation).r;
                    float3 posWorld = SphericalEnvmapToDirection(sampleLocation);
                    float cosTheta = dot(normal, posWorld);
                    if (cosTheta > 0.0 && pdf > 0.0) 
                    {
                        float theta = PI * (1.0 - sampleLocation.y);
                        result += 2.0 * PI * radiance * cosTheta * sin(theta) / pdf;
                    }
                }

                result = result / float(N);
                
                return result;
            }

            float4 FragMain(PSInput input) : SV_Target
            {
                float3 color = PreFilterEnvMapDiffuse(input.uv);
                return float4(color, 1);
            }

            #endif

            ENDHLSL
        }

        Pass
        {
            Name "Specular"
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

            cbuffer _Constants : SRG_PerMaterial(b2)
            {
                float _Roughness;
            };

            // convert texture coordinates to pixels
            inline float t2p(in float t, in int noOfPixels) {
                return t * float(noOfPixels) - 0.5;
            }

            // from http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
            // Hacker's Delight, Henry S. Warren, 2001
            float radicalInverse(uint bits) {
                bits = (bits << 16u) | (bits >> 16u);
                bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
                bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
                bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
                bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
                return float(bits) * 2.3283064365386963e-10; // / 0x100000000
            }

            inline float2 hammersley(uint n, uint N) {
                return float2(float(n) / float(N), radicalInverse(n));
            }

            float3 SphericalEnvmapToDirection(float2 tex) {
                float theta = PI * (1.0 - tex.y);
                float phi = 2.0 * PI * (0.5 - tex.x);
                return float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            }

            float2 DirectionToSphericalEnvmap(float3 dir) {
                float phi = atan2(dir.y, dir.x);
                float theta = acos(dir.z);
                float s = 0.5 - phi / (2.0 * PI);
                float t = 1.0 - theta / PI;
                return float2(s, t);
            }

            float3x3 GetNormalFrame(in float3 normal) {
                float3 someVec = float3(1.0, 0.0, 0.0);
                float dd = dot(someVec, normal);
                float3 tangent = float3(0.0, 1.0, 0.0);
                if(1.0 - abs(dd) > 1e-6) {
                    tangent = normalize(cross(someVec, normal));
                }
                float3 bitangent = cross(normal, tangent);
                return float3x3(tangent, bitangent, normal);
            }

            float Fill(float sdf) {
                return clamp(0.5 - sdf / fwidth(sdf), 0.0, 1.0);
            }

            float Circle(float2 pos, float2 center, float radius) {
                return length(pos - center) - radius;
            }

            float3 ImportanceSampleGGX( float2 Xi, float Roughness, float3 N, out float CosTheta, out float SinTheta )
            {
                float a = Roughness * Roughness;
                float Phi = 2.0 * PI * Xi.x;
                CosTheta = sqrt( (1.0 - Xi.y) / ( 1.0 + (a*a - 1.0) * Xi.y ) );
                SinTheta = sqrt( 1.0 - CosTheta * CosTheta );
                float3 H;
                H.x = SinTheta * cos( Phi );
                H.y = SinTheta * sin( Phi );
                H.z = CosTheta;
                float3 UpVector = abs(N.z) < 0.999 ? float3(0,0,1) : float3(1,0,0);
                float3 TangentX = normalize( cross( UpVector, N ) );
                float3 TangentY = cross( N, TangentX );
                // Tangent to world space
                return TangentX * H.x + TangentY * H.y + N * H.z;
            }

            float specularD(float roughness, float NoH)
            {
                float r2 = roughness * roughness;
                float NoH2 = NoH * NoH;
                float a = 1.0/(3.14159*r2*pow(NoH, 4.0));
                float b = exp((NoH2 - 1.0) / r2 * NoH2);
                return  a * b;
            }

            float3 PreFilterEnvMapSpecular(in float2 tex)
            {
                uint width;
                uint height;
                _InputTexture.GetDimensions(width, height);
                float roughness = clamp(_Roughness, 0.0, 1.0);
                float alpha = roughness * roughness;
                float alpha2 = alpha * alpha;

                float px = t2p(tex.x, width);
                float py = t2p(tex.y, height);
                float3 normal = SphericalEnvmapToDirection(tex);
                float3 result = float3(0, 0, 0);
                float weight = 0.0;
                float3 N = normal;
                float3 V = N;

                const uint SampleCount = 3096;

                for(uint n = 0u; n < SampleCount; n++) 
                {
                    float2 Xi = hammersley(n, SampleCount);
                    float cosTheta;
                    float sinTheta;
                    
                    float3 H = ImportanceSampleGGX( Xi, roughness, N, cosTheta, sinTheta );
                    float3 L = 2.0 * dot( V, H ) * H - V;
                    float2 uv = DirectionToSphericalEnvmap(L);
                    float NoL = clamp(dot( N, L ), 0.0, 1.0);
                    float VoL = clamp(dot( V, L ), 0.0, 1.0);
                    float NoH = clamp(dot( N, H ), 0.0, 1.0);
                    float VoH = clamp(dot( V, H ), 0.0, 1.0);
                    float NoV = clamp(dot( N, V ), 0.0, 1.0);

                    if (NoL > 0.0)
                    {
                        float Dh = specularD(roughness, NoH);
                        float num = cosTheta * cosTheta * (alpha2 - 1.0) + 1.0;
                        float pdf = alpha2 * cosTheta * sinTheta / (PI * num * num);
                        float pdf2 = Dh * NoH / (4.0 * VoH);
                        float t = alpha + 0.0;
                        // Lerp the PDFs for better results in low light high frequency scenes. Ex: A nigh cubemap with an extremely bright street lamp.
                        // For daylight scenes, the results are almost same for both PDFs.
                        pdf = lerp(pdf, pdf2, t);

                        float solidAngleTexel = 4.0 * PI / (6.0 * float(height) * float(height));
                        float solidAngleSample = 1.0 / (float(SampleCount) * pdf);
                        float lod = 0.5 * log2(4.0 * solidAngleSample / solidAngleTexel);
                        if (roughness <= 1e-8)
                        {
                            lod = 0.0;
                        }
                        //lod = clamp(lod, 0, 7);

                        result += _InputTexture.SampleLevel(_InputSampler, uv, lod).rgb * NoL;
                        weight += NoL;
                    }
                }
                result = result / weight;
                return result;
            }

            float4 FragMain(PSInput input) : SV_Target
            {
                float3 color = PreFilterEnvMapSpecular(input.uv);
                return float4(color, 1);
            }

            #endif

            ENDHLSL
        }
    }
}