#version 450

layout(location = 0) in vec3 inWorldPosition;

layout(binding = 1) uniform samplerCube sky;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 direction = normalize(inWorldPosition);
    outColor = texture(sky, inWorldPosition);
}