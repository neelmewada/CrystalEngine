#ifndef __SCENE_DATA_HLSL__
#define __SCENE_DATA_HLSL__

#include "LightData.hlsli"

TextureCube<float4> _Skybox : SRG_PerScene(t);
SamplerState _DefaultSampler : SRG_PerScene(t);

#endif
