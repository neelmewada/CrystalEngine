#ifndef __SCENE_DATA_HLSL__
#define __SCENE_DATA_HLSL__

#include "Lighting.hlsl"

cbuffer _SceneData : SRG_PerScene(b4)
{
    float _TimeElapsed;
};

TextureCube<float4> _Skybox : SRG_PerScene(t5);
SamplerState _DefaultSampler : SRG_PerScene(t6);

#endif
