
#pragma target 6.0


struct VertexInput
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float3 color : TEXCOORD0;
};

VertexOutput vert(VertexInput i)
{
    VertexOutput o;
    o.pos = float4(i.pos, 1.0);
    o.color = i.color;
    return o;
}

float4 frag(VertexOutput i) : SV_TARGET
{
    return float4(i.color, 1.0);
}
