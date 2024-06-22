#ifndef __MACROS_HLSL__
#define __MACROS_HLSL__

// TODO: Temp code
//#define LIMITED_SETS 1

#define PI 3.1415926535897932384626433832795

// Helpful for intellisense
#ifndef COMPILE
#define VERTEX 1
#define FRAGMENT 1
#endif


#ifdef LIMITED_SETS // Almost all android phones only support maxBoundDescriptorSets = 4

#ifndef PerScene_Frequency
#define PerScene_Frequency 0
#endif

#ifndef PerView_Frequency
#define PerView_Frequency 0
#endif

#ifndef PerPass_Frequency
#define PerPass_Frequency 0
#endif

#ifndef PerSubPass_Frequency
#define PerSubPass_Frequency 0
#endif

#ifndef PerMaterial_Frequency
#define PerMaterial_Frequency 1
#endif

#ifndef PerObject_Frequency
#define PerObject_Frequency 2
#endif

#ifndef PerDraw_Frequency
#define PerDraw_Frequency 3
#endif

#else // All platforms other than android support minimum maxBoundDescriptorSets = 8 for majority of devices

#ifndef PerScene_Frequency
#define PerScene_Frequency 0
#endif

#ifndef PerView_Frequency
#define PerView_Frequency 1
#endif

#ifndef PerPass_Frequency
#define PerPass_Frequency 2
#endif

#ifndef PerSubPass_Frequency
#define PerSubPass_Frequency 3
#endif

#ifndef PerMaterial_Frequency
#define PerMaterial_Frequency 4
#endif

#ifndef PerObject_Frequency
#define PerObject_Frequency 5
#endif

#ifndef PerDraw_Frequency
#define PerDraw_Frequency 6
#endif

#endif

#define EXPAND(x) x
#define CONCATENATE(arg1, arg2)   __CONCATENATE1(arg1, arg2)
#define __CONCATENATE1(arg1, arg2)  __CONCATENATE2(arg1, arg2)
#define __CONCATENATE2(arg1, arg2)  arg1 ## arg2

//#define SRG(type, frequencyId) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, frequencyId)))
#define SRG(type, frequencyId) register(type, EXPAND(CONCATENATE(space, frequencyId)))

#define SRG_PerScene(type) SRG(type, PerScene_Frequency)
#define SRG_PerView(type) SRG(type, PerView_Frequency)
#define SRG_PerPass(type) SRG(type, PerPass_Frequency)
#define SRG_PerSubPass(type) SRG(type, PerSubPass_Frequency)
#define SRG_PerMaterial(type) SRG(type, PerMaterial_Frequency)
#define SRG_PerObject(type) SRG(type, PerObject_Frequency)
#define SRG_PerDraw(type) SRG(type, PerDraw_Frequency)

#ifdef __spirv__ // Vulkan Shader

#define SUBPASS_INPUT(subpass, name) [[vk::input_attachment_index(subpass)]] [[vk::binding(subpass, PerSubPass_Frequency)]] SubpassInput name
#define SUBPASS_LOAD(subpassName) subpassName.SubpassLoad()

#define BEGIN_ROOT_CONSTANTS() struct RootConstants {
#define END_ROOT_CONSTANTS() }; [[vk::push_constant]] RootConstants _RootConstants;
#define ROOT_CONSTANT(name) _RootConstants.name

#define IMAGE_FORMAT(format) [[vk::image_format(#format)]]

#else // Dx12 Shader

#define SUBPASS_INPUT(subpass, name) SubpassInput name;
#define SUBPASS_LOAD(subpassName) subpassName.SubpassLoad()

#define IMAGE_FORMAT(format)

#endif


float4x4 inverse(float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

inline float clamp01(float value)
{
    return clamp(value, 0, 1);
}

#endif // __MACROS_HLSL__