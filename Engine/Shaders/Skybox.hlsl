
#undef USE_INSTANCING

#include "Core/Macros.hlsli"
#include "Core/SceneData.hlsli"
#include "Core/ViewData.hlsli"
#include "Core/ObjectData.hlsli"

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
    direction.y *= -1;
    float3 cubemapSample = _Skybox.Sample(_DefaultSampler, direction).rgb;
    return float4(lerp(float3(1.0, 0, 1.0), _Skybox.Sample(_DefaultSampler, direction).rgb, 0.9999), 1);
}
#endif
