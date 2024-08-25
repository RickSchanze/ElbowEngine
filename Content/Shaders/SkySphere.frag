#version 450

layout(location = 0) in vec3 inWorldPosition;

layout(binding = 1) uniform sampler2D sky;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 direction = normalize(inWorldPosition);
    vec2 uv = direction.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5);
    outColor = texture(sky, uv);
}
