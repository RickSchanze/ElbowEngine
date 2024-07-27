#version 450
#extension GL_EXT_debug_printf : enable

layout(binding = 0) uniform UboView {
    mat4 projection;
    mat4 view;
} ubo_view;

layout(binding = 1) uniform UboInstance {
    mat4 model;
} ubo_instance;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec2 outUV;

void main() {
    gl_Position = ubo_view.projection * ubo_view.view * ubo_instance.model * vec4(inPosition, 1.0);
    outUV = inUV;
}
