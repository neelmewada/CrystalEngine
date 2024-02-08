#ifndef __SCENE_DATA_HLSL__
#define __SCENE_DATA_HLSL__

#include "LightData.hlsli"

cbuffer _SceneData : SRG_PerScene(b3)
{
    float _TimeElapsed;
};

TextureCube<float4> _Skybox : SRG_PerScene(t4);
SamplerState _DefaultSampler : SRG_PerScene(t5);

#endif
