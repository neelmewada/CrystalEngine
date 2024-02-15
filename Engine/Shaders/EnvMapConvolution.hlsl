
#include "Core/Macros.hlsli"

#include "Core/Macros.hlsli"
#include "Core/Gamma.hlsli"


struct VSInput
{
    float3 position : POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 localPosition : TEXCOORD0;
};


cbuffer _PerViewData : SRG_PerPass(b0)
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
    float3 viewPosition;
};

#if VERTEX
PSInput VertMain(VSInput input)
{
    PSInput o;
    o.localPosition = input.position;
    o.position = mul(float4(input.position, 1.0), viewProjectionMatrix);
    return o;
}
#endif


#if FRAGMENT

// Mirror binary digits about the decimal point
float radicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Randomish sequence that has pretty evenly spaced points
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float2 Hammersley(uint i, uint N)
{
    return float2(float(i)/float(N), radicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;

    float Phi = 2 * PI * Xi.x;
    float cosTheta = sqrt( (1 - Xi.y) / ( 1 + (a * a - 1) * Xi.y ) ); 
    float sinTheta = sqrt( 1 - cosTheta * cosTheta );

    float3 H;
    H.x = sinTheta * cos(Phi);
    H.y = sinTheta * sin(Phi);
    H.z = cosTheta;

    float3 upVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangentX = normalize(cross(upVector, N));
    float3 tangentY = cross(N, tangentX);
    return tangentX * H.x + tangentY * H.y + N * H.z;;
}

TextureCube<float4> _CubeMap : SRG_PerPass(t1);
SamplerState _CubeMapSampler : SRG_PerPass(t2);

struct SH9
{
    float coeff[9];
};

struct SH9Color
{
    float3 color[9];
};

SH9 GenSHCoefficients(float3 N)
{
    SH9 result;
 
    // Band 0
    result.coeff[0] = 0.282095f;
 
    // Band 1
    result.coeff[1] = 0.488603 * N.y;
    result.coeff[2] = 0.488603 * N.z;
    result.coeff[3] = 0.488603 * N.x;
 
    // Band 2
    result.coeff[4] = 1.092548 * N.y * N.z;
    result.coeff[5] = 0.315392 * (-N.x * N.x - N.y * N.y + 2 * N.z * N.z);
    result.coeff[6] = 0.546274 * N.z * (3 * N.x * N.x - 1);
    result.coeff[7] = 0.546274 * N.x * N.x - N.y * N.y;
    result.coeff[8] = 0.273137 * N.x * N.y;
 
    return result;
}

SH9Color GenLightingCoefficients(float3 N, float3 L)
{
    SH9 coeff = GenSHCoefficients(N);
    SH9Color result;
    for (int i = 0; i < 9; i++)
    {
        result.color[i] = L * coeff.coeff[i];
    }
    return result;
}

float3 EvaluateSHIrradiance( float3 _Direction, float3 _SH[9] ) {
    const float c1 = 0.42904276540489171563379376569857;    // 4 * Â2.Y22 = 1/4 * sqrt(15.PI)
    const float c2 = 0.51166335397324424423977581244463;    // 0.5 * Â1.Y10 = 1/2 * sqrt(PI/3)
    const float c3 = 0.24770795610037568833406429782001;    // Â2.Y20 = 1/16 * sqrt(5.PI)
    const float c4 = 0.88622692545275801364908374167057;    // Â0.Y00 = 1/2 * sqrt(PI)

    float   x = _Direction.x;
    float   y = _Direction.y;
    float   z = _Direction.z;

    return  max( 0.0,
            (c1*(x*x - y*y)) * _SH[8]                       // c1.L22.(x²-y²)
            + (c3*(3.0*z*z - 1)) * _SH[6]                   // c3.L20.(3.z² - 1)
            + c4 * _SH[0]                                   // c4.L00 
            + 2.0*c1*(_SH[4]*x*y + _SH[7]*x*z + _SH[5]*y*z) // 2.c1.(L2-2.xy + L21.xz + L2-1.yz)
            + 2.0*c2*(_SH[3]*x + _SH[1]*y + _SH[2]*z) );    // 2.c2.(L11.x + L1-1.y + L10.z)
}

float3 GetBase(float3 direction, int index) {
    float x = direction[0];
    float y = direction[1];
    float z = direction[2];
    
    if (index == 0)
        return float3(
        0.282095, 
        -0.488603 * y, 
        0.488603 * z
        );
    if (index == 1)
        return float3(
        -0.488603 * x,
        1.092548 * x*y,
        -1.092548 * y*z
        );
    if (index == 2)
        return float3(
        0.315392 * (3.0*z*z - 1.0),
        -1.092548 * x*z,
        0.546274 * (x*x - y*y)
        );

    return float3(0, 0, 0);
}

float3 CalculateSH9Values(float3 normal)
{
    float3 irradiance = float3(0, 0, 0);
    float3 SH[9];
    SH[0] = float3(2.58676, 2.730808, 3.152812);
    SH[1] = float3(-0.431493, -0.665128, -0.969124);
    SH[2] = float3(-0.353886, 0.048348, 0.672755);
    SH[3] = float3(-0.604269, -0.88623, -1.298684);
    SH[4] = float3(0.320121, 0.422942, 0.541783);
    SH[5] = float3(-0.137435, -0.168666, -0.229637);
    SH[6] = float3(-0.052101, -0.149999, -0.232127);
    SH[7] = float3(-0.117312, -0.167151, -0.265015);
    SH[8] = float3(-0.090028, -0.021071, 0.08956);

    float3 base0 = GetBase(normal, 0);
    float3 base1 = GetBase(normal, 1);
    float3 base2 = GetBase(normal, 2);

    for (int z = 0; z < 3; ++z)
    {
        irradiance[z]  = dot(SH[z*3], base0) + dot(SH[z*3 + 1], base1) + dot(SH[z*3 + 2], base2);
		irradiance[z]  = max(irradiance[z], 0.0);
    }

    irradiance = irradiance / PI;

    return irradiance;
}

float4 FragMain(PSInput input) : SV_Target
{
    float3 normal = normalize(input.localPosition);
    float3 irradiance = float3(0, 0, 0);
    float3 N = normal;

    //return float4(_CubeMap.Sample(_CubeMapSampler, N).rgb, 1.0); // For debugging purposes

    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleDelta = 0.005;
    float nrSamples = 0.0;
    const uint sampleCount = 16384;

    float totalWeight = 0.0f;
    uint w;
    uint h;
    uint numLevels;
    _CubeMap.GetDimensions(0, w, h, numLevels);

    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            float3 color = _CubeMap.SampleLevel(_CubeMapSampler, sampleVec, 5).rgb * cosTheta * sinTheta;
            
            irradiance += color;
            totalWeight += 1;
        }
    }

    irradiance = (PI * irradiance) / totalWeight;

    return float4(irradiance, 1);
}

#endif


