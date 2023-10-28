#ifndef __MACROS_H__
#define __MACROS_H__

#define SRG(pos, set) register(pos, space##set)

#ifndef PerScene_Frequency
#define PerScene_Frequency 0
#endif

#ifndef PerView_Frequency
#define PerView_Frequency 0
#endif

#ifndef PerPass_Frequency
#define PerPass_Frequency 1
#endif

#ifndef PerMaterial_Frequency
#define PerMaterial_Frequency 2
#endif

#ifndef PerObject_Frequency
#define PerObject_Frequency 3
#endif

#ifndef PerDraw_Frequency
#define PerDraw_Frequency 3
#endif

#define EXPAND(x) x
#define CONCATENATE(arg1, arg2)   __CONCATENATE1(arg1, arg2)
#define __CONCATENATE1(arg1, arg2)  __CONCATENATE2(arg1, arg2)
#define __CONCATENATE2(arg1, arg2)  arg1 ## arg2

#define SRG(frequencyId, type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, frequencyId)))

#define SRG_PerScene(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerScene_Frequency)))
#define SRG_PerView(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerView_Frequency)))
#define SRG_PerPass(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerPass_Frequency)))
#define SRG_PerMaterial(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerMaterial_Frequency)))
#define SRG_PerObject(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerObject_Frequency)))
#define SRG_PerDraw(type) register(EXPAND(CONCATENATE(type, __COUNTER__)), EXPAND(CONCATENATE(space, PerDraw_Frequency)))

#endif // __MACROS_H__