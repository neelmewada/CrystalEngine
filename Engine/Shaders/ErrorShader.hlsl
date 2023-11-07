
// data structure : before vertex shader (mesh info)
struct VertexInfo
{
    float3 position : POSITION;
};

struct v2f
{
    float4 position : SV_POSITION;
};

struct PerViewData
{
    float4x4 viewMatrix;
    float4x4 viewProjectionMatrix;
    float4x4 projectionMatrix;
};

struct ModelData
{
    float4x4 modelMatrix;
};

ConstantBuffer<PerViewData> _PerViewData : register(b0, space0);

ConstantBuffer<ModelData> _Model : register(b0, space1);

// vertex shader function
v2f VertMain(VertexInfo input)
{
    v2f output;
    output.position = mul(float4(input.position, 1.0), mul(_PerViewData.projectionMatrix, mul(_PerViewData.viewMatrix, _Model.modelMatrix)));
    //output.position = float4(input.position, 1.0);
    return output;
}

// pixel shader function
float4 FragMain(v2f input) : SV_TARGET
{
    return float4(1.0, 0, 1.0, 1.0);
}

