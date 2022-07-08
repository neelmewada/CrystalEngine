#version 450  // GLSL v4.5

// Uniforms that are same for all objects in scene
layout (set = 0, binding = 0) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
} globals;

// Per-object uniforms
//layout (set = 1, binding = 0) uniform Object {
//    mat4 model;
//    uint objectBufferIndex; // Index of material in the object storage buffer
//} object;

layout (push_constant) uniform PushConstants {
    mat4 model;
} push;


#ifdef VERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 outColor;

void main() {
    gl_Position = globals.projection * globals.view * push.model * vec4(vPosition, 1.0);
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
