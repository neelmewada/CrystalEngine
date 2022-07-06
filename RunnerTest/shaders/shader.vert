#version 450  // GLSL v4.5


layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 outColor;

layout (binding = 0) uniform MVP {
    mat4 projection;
    mat4 view;
    mat4 model;
} mvp;

void main() {
    gl_Position = mvp.projection * mvp.view * mvp.model * vec4(vPosition, 1.0);
    outColor = vColor;
}

