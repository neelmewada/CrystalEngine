#ifndef __MACROS_HLSL__
#define __MACROS_HLSL__

#define LIMITED_SETS 1

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

#define SRG(type, frequencyId) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, frequencyId)))

#define SRG_PerScene(type) SRG(type, PerScene_Frequency)
#define SRG_PerView(type) SRG(type, PerView_Frequency)
#define SRG_PerPass(type) SRG(type, PerPass_Frequency)
#define SRG_PerSubPass(type) SRG(type, PerSubPass_Frequency)
#define SRG_PerMaterial(type) SRG(type, PerMaterial_Frequency)
#define SRG_PerObject(type) SRG(type, PerObject_Frequency)
#define SRG_PerDraw(type) SRG(type, PerDraw_Frequency)

#ifdef __spirv__
#define SUBPASS_INPUT(subpass, name) [[vk::input_attachment_index(subpass)]] [[vk::binding(subpass)]] SubpassInput name;
#define ROOT_CONSTANT() [[vk::push_constant]]
#else
#define SUBPASS_INPUT(subpass, name) SubpassInput name;
#define ROOT_CONSTANT()
#endif

#endif // __MACROS_HLSL__