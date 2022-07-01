#version 450  // GLSL v4.5

layout (location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor; // Final output color

void main() {
    outColor = vec4(inColor, 1.0);
}
