#version 450  // GLSL v4.5

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 outColor;


void main() {
    gl_Position = vec4(vPosition, 1.0);
    outColor = vColor;
}

