#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outWorldPos;
layout(location = 1) out vec3 outLightPos;

layout(binding = 0) uniform UboView {
    mat4 projection;
    mat4 light;
} ubo_view;

layout(push_constant) uniform CameraView {
    mat4 view;
} camera_view;

layout(binding = 1) uniform UboInstance {
    mat4 model;
} ubo_instance;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = ubo_view.projection * camera_view.view * ubo_instance.model * vec4(inPosition, 1.0);

    outWorldPos = ubo_instance.model * vec4(inPosition, 1.0);
    outLightPos = ubo_view.light[0].xyz;
}
