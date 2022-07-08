#version 450  // GLSL v4.5

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 outColor;

// Properties that are same for all objects in scene
layout (binding = 0) uniform Global {
    mat4 projection;
    mat4 view;
} global;

// Per-object properties
layout (binding = 1) uniform Local {
    mat4 model;
} local;

void main() {
    gl_Position = global.projection * global.view * local.model * vec4(vPosition, 1.0);
    outColor = vColor;
}

