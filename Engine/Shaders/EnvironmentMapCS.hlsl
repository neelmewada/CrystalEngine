
#include "Core/Macros.hlsli"
#include "Core/Gamma.hlsli"

TextureCube<float4> _EnvMap : SRG_PerPass(t0);
SamplerState _CubeMapSampler : SRG_PerPass(s1);


[numthreads(1, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    
}
