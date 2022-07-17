#version 450  // GLSL v4.5

#define STATIC(location) layout(set = 0, binding = location)
#define PER_FRAME(location) layout(set = 1, binding = location)
#define PER_PIPELINE(location) layout(set = 2, binding = location)
#define DYNAMIC(location) layout(set = 3, binding = location)

#define SET2(location) layout(set = 2, binding = location)
#define SET3(location) layout(set = 3, binding = location)

STATIC(0) uniform GlobalUniformBuffer {
    mat4 projection;
    mat4 view;
};

PER_FRAME(0) uniform ObjectBuffer {
    mat4 model;
    vec3 colorTint;
};

PER_FRAME(1) uniform sampler2D tex;

#ifdef VERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vUV;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outColor;

void main() {
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
    outColor = vColor;
    outUV = vUV;
}

#endif

#ifdef FRAGMENT

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inColor;

layout(location = 0) out vec4 outColor; // Final output color

void main() {
    outColor = vec4(texture(tex, inUV).rgb, 1.0);
}

#endif
