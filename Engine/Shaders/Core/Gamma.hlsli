#ifndef __GAMMA_HLSL__
#define __GAMMA_HLSL__

// Constants
static const float gamma = 2.2;
static const float invGamma = 1.0 / gamma;

inline float3 GammaToLinear(float3 linearColor) {
    return pow(linearColor, float3(gamma, gamma, gamma));
}

inline float3 LinearToGamma(float3 srgbColor) {
    return pow(srgbColor, float3(invGamma, invGamma, invGamma));
}


#endif