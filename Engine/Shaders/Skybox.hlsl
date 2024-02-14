
#undef USE_INSTANCING

#include "Core/Macros.hlsli"
#include "Core/SceneData.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/ObjectData.hlsli"

#include "Core/Gamma.hlsli"

struct VSInput
{
    float3 position : POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

#if VERTEX
PSInput VertMain(VSInput input)
{
    PSInput o;
    o.worldPos = LOCAL_TO_WORLD_SPACE(float4(input.position, 1.0), input).xyz;
    //o.worldPos += viewPosition;
    o.position = WORLD_TO_CLIP_SPACE(float4(o.worldPos, 1.0));
    return o;
}
#endif

#if FRAGMENT
float4 FragMain(PSInput input) : SV_TARGET
{
    float3 direction = normalize(input.worldPos - viewPosition);
    float3 color = _Skybox.Sample(_DefaultSampler, direction).rgb;
    color = color / (color + float3(1.0, 1.0, 1.0) * 0.5); // HDR tonemapping (optional)
    return float4(color, 1);
}
#endif
