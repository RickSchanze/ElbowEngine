#version 450

layout(location = 0) in vec3 inPosition;

layout(binding = 0) uniform UboView {
    mat4 projection;
    mat4 view;
} ubo_view;

layout(location = 0) out vec3 outWorldPosition;

void main() {
    mat4 viewNoTranslation = mat4(mat3(ubo_view.view));
    gl_Position = ubo_view.projection * viewNoTranslation * vec4(inPosition, 1.0);
    outWorldPosition = inPosition;
}
