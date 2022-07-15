#version 450  // GLSL v4.5

#define GLOBAL(location) layout(set = 0, binding = location)
#define PERPASS(location) layout(set = 1, binding = location)
#define MATERIAL(location) layout(set = 2, binding = location)
#define INSTANCE(location) layout(set = 3, binding = location)

#define SET1(location) layout(set = 1, binding = location)
#define SET2(location) layout(set = 2, binding = location)
#define SET3(location) layout(set = 3, binding = location)


#define MAX_INSTANCES 1023

struct MaterialData {
    uint tex1Idx;
    uint tex2Idx;
    vec4 color;
};

struct ObjectData {
    mat4 model;
    uint materialIndex;
};

struct LightData {
    vec4 lightColor;
    vec3 lightPos;
};

GLOBAL(0) uniform GlobalUniformBuffer {
    mat4 projection;
    mat4 view;
};

//SET1(0) uniform ObjectBuffer {
//    mat4 model;
//    vec3 colorTint;
//};

/*
// Set1: Per-Pass Data
layout(std140, set = 1, binding = 0) uniform buffer PerPassData {
    float someValue;
};

// Set2: Per-Pipeline Data
layout(std140, set = 2, binding = 0) readonly buffer MaterialBuffer {
    MaterialData materials[];
};

// Set3: Per-Instance Data
layout(std140, set = 3, binding = 0) readonly buffer ObjectBuffer {
    uint objectIndices[];
    ObjectData objects[]; // access using: objects[objectIndices[gl_InstanceIndex]]
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

