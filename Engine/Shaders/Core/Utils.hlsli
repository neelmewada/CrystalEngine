#ifndef __UTILS_HLSL__
#define __UTILS_HLSL__

// from http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// Hacker's Delight, Henry S. Warren, 2001
float RadicalInverse(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

inline float2 Hammersley(uint n, uint N) {
    return float2(float(n) / float(N), RadicalInverse(n));
}

#endif // __UTILS_HLSL__
