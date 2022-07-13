#version 450  // GLSL v4.5

#define STATIC(location) layout(set = 0, binding = location)
#define MUTABLE(location) layout(set = 1, binding = location)
#define DYNAMIC(location) layout(set = 2, binding = location)

// STATIC
STATIC(0) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
    vec4 test;
    float test2;
    uint integers;
};

struct MaterialData {
    uint tex1Idx;
    uint tex2Idx;
    uint tex3Idx;
};

struct PerObjectData {
    mat4 model;
    uint objectIndex;
    uint materialIndex;
};

#ifdef VERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 outColor;

void main() {
    gl_Position = projection * view * vec4(vPosition, 1.0);
    outColor = vColor;
}

#endif

#ifdef FRAGMENT

layout (location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor; // Final output color

void main() {
    outColor = vec4(inColor, 1.0);
}

#endif

/*
// set0: STATIC
layout(set = 0, binding = 0) uniform mat4 view;
layout(set = 0, binding = 1) uniform mat4 projection;
layout(set = 0, binding = 2) uniform vec4 dirLightColor;
--OR--
layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 view;
    mat4 projection;
    vec4 dirLightColor;
};

// set1: MUTABLE (they are Dynamic Uniform/Storage Buffers)
layout(set = 1, binding = 0) uniform sampler2D textures[];

*/

/*
// Below 2 are same for all shaders
layout(set = 0, binding = 1) readonly buffer MaterialBuffer { // Dynamic Offset Storage Buffer
    MaterialData materials[];
};
layout(set = 0, binding = 2) readonly buffer ObjectBuffer { // Storage Buffer
    PerObjectData objects[];
};

// Per Shader textures
layout(set = 1, binding = 0) uniform sampler2D textures[5]; // Max 5 textures per material

// Per-DrawCall Uniform Buffer
layout(std140, set = 2, binding = 0) uniform DrawCall {
    uint objectIndices[];
};

layout(push_constant) uniform Constants {
    mat4 model;
};
*/

