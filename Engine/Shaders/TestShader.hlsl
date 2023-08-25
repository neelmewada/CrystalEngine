//#define SRG(setNum, bindingNum) [[vk::binding(bindingNum, setNum)]]

#define SRG(pos, set) register(pos, space##set)

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


ConstantBuffer<MyBuffer> buff : SRG(b0, 0); //register(b0, space0);
ConstantBuffer<MyBuffer> buff2 : SRG(b1, 0);

Texture2D texture : SRG(t0, 1);
SamplerState textureSampler : SRG(t1, 1);

// vertex shader function
v2p VertMain(VertexInfo input)
{
    v2p output;
    output.position = float4(input.position * buff.cameraPos * buff2.cameraPos, 1.0);
    return output;
}

// pixel shader function
float4 FragMain(v2p input) : SV_TARGET
{
    float4 col = texture.Sample(textureSampler, float2(0, 0));
    return float4(buff.lightColor * col.rgb, 0.0);
}
