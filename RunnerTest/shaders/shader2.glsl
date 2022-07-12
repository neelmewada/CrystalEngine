#version 450  // GLSL v4.5

// Uniforms that are same for all objects in scene

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
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

/*
layout(set = 1, binding = 0) uniform sampler2D textures[4]; // Max 4 textures per material
layout(std140, set = 1, binding = 1) readonly buffer MaterialBuffer { // Dynamic Offset Storage Buffer
    MaterialData materials[];
};
layout(std140, set = 1, binding = 2) readonly buffer ObjectBuffer { // Storage Buffer
    PerObjectData objects[];
};

// Per-DrawCall Uniform Buffer
layout(std140, set = 2, binding = 0) uniform DrawCall {
    uint objectIndices[];
};

layout(push_constant) uniform Constants {
    mat4 model;
};
*/

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
