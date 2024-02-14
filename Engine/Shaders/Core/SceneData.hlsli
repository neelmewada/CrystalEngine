#ifndef __SCENE_DATA_HLSL__
#define __SCENE_DATA_HLSL__

#include "Lighting.hlsl"

cbuffer _SceneData : SRG_PerScene(b4)
{
    float _TimeElapsed;
};

#endif
