#version 450

layout(location = 0) in vec3 inWorldPosition;

layout(binding = 1) uniform sampler2D sky;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 direction = normalize(inWorldPosition);
    vec2 uv = vec2(atan(direction.x, direction.z) / (2.0 * 3.14159265359), acos(-direction.y));
    outColor = texture(sky, uv);
}
