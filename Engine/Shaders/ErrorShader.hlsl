
// data structure : before vertex shader (mesh info)
struct VertexInfo
{
    float3 position : POSITION;
};

struct v2f
{
    float4 position : SV_POSITION;
};

struct ModelData
{
    float4x4 modelMatrix;
    float4x4 modelViewMatrix;
    float4x4 modelViewProjectionMatrix;
};

ConstantBuffer<ModelData> _Model : register(b0, space0);

// vertex shader function
v2f VertMain(VertexInfo input)
{
    v2f output;
    output.position = mul(float4(input.position, 1.0), _Model.modelViewProjectionMatrix);
    return output;
}

// pixel shader function
float4 FragMain(v2f input) : SV_TARGET
{
    return float4(1.0, 0, 1.0, 1.0);
}

