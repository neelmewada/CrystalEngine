#version 450

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

#define SRG(frequencyId, location, ...) layout(set = frequencyId, binding = location, ##__VA_ARGS__)

#define SRG_PerScene(location) 
#define SRG_PerView(location, ...) SRG(PerView_Frequency, location, ##__VA_ARGS__)
#define SRG_PerPass(location) 
#define SRG_PerMaterial(location)
#define SRG_PerObject(location) 
#define SRG_PerDraw(location)

SRG(0, 0) uniform PerViewData {
    mat4 viewMatrix;
    mat4 viewProjectionMatrix;
}

SRG(1, 0) uniform ObjectData {
    mat4 modelMatrix;
}

#if VERTEX

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

void main()
{
    gl_Position = modelMatrix * viewProjectionMatrix * position;
}

#elif FRAGMENT

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(1, 0, 1, 1);
}

#endif
