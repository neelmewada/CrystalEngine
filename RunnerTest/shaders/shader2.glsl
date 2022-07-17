#version 450  // GLSL v4.5

#define STATIC(location) layout(set = 0, binding = location)
#define PER_PIPELINE(location) layout(set = 1, binding = location)

#define DYNAMIC0_std140(location) layout(std140, set = 2, binding = location)
#define DYNAMIC0(location) layout(set = 2, binding = location)

#define DYNAMIC1(location) layout(set = 3, binding = location)
#define DYNAMIC1_std140(location) layout(std140, set = 3, binding = location)

#define SET2(location) layout(set = 2, binding = location)
#define SET3(location) layout(set = 3, binding = location)

#define MAX_INSTANCES 255

struct MaterialData {
    uint tex1Idx;
    uint tex2Idx;
    vec4 color;
};

struct ObjectData {
    mat4 model;
};

struct LightData {
    vec4 lightColor;
    vec3 lightPos;
};

STATIC(0) uniform GlobalUniformBuffer {
    mat4 projection;
    mat4 view;
};

DYNAMIC0_std140(0) uniform ObjectBuffer {
    ObjectData objects[MAX_INSTANCES];
};

DYNAMIC0(1) uniform sampler2D tex;

DYNAMIC0_std140(2) uniform InstanceBuffer {
    uint instances[MAX_INSTANCES];
};

#ifdef VERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vUV;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outColor;
layout (location = 2) out float outInt;

void main() {
    gl_Position = projection * view * objects[instances[gl_InstanceIndex]].model * vec4(vPosition, 1.0);
    outColor = vColor;
    outUV = vUV;
    outInt = instances[gl_InstanceIndex];
}

#endif

#ifdef FRAGMENT

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inColor;
layout (location = 2) in float inInt;

layout(location = 0) out vec4 outColor; // Final output color

void main() {
    outColor = vec4(texture(tex, inUV).rgb, 1.0);
}

#endif

