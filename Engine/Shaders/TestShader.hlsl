#define SRG(setNum, bindingNum) [[vk::binding(bindingNum, setNum)]]

// data structure : before vertex shader (mesh info)
struct VertexInfo
{
    float3 position : POSITION;
};

// data structure : vertex shader to pixel shader
// also called interpolants because values interpolates through the triangle
// from one vertex to another
struct v2p
{
    float4 position : SV_POSITION;
    float3 uv : TEXCOORD0;
    float3 color : TEXCOORD1;
};

struct MyBuffer
{
    float3 lightColor;
    float3 cameraPos;
};

SRG(0, 1)
ConstantBuffer<MyBuffer> buff;

// vertex shader function
v2p VertMain(VertexInfo input)
{
    v2p output;
    output.position = float4(input.position * buff.cameraPos, 1.0);
    return output;
}

// pixel shader function
float4 FragMain(v2p input) : SV_TARGET
{
    return float4(buff.lightColor, 0.0);
}
