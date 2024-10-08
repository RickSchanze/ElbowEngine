#version 450
#extension GL_EXT_debug_printf : enable

layout(binding = 0) uniform UboView {
    mat4 projection;
    mat4 view;
    mat4 camera;
} ubo_view;

layout(binding = 1) uniform UboInstance {
    mat4 model;
} ubo_instance;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outFragPosition;
layout(location = 3) out vec3 outCameraPosition;
layout(location = 4) out vec4 outWorldPosition;

void main() {
    gl_Position = ubo_view.projection * ubo_view.view * ubo_instance.model * vec4(inPosition, 1.0);

    outUV = inUV;
    outFragPosition = vec3(ubo_instance.model * vec4(inPosition, 1.0));
    // 法线乘以法线矩阵
    // 防止因不规则形变而导致的法线方向错误
    outNormal = mat3(transpose(inverse(ubo_instance.model))) * inNormal;
    outCameraPosition = ubo_view.camera[0].xyz; // 相机位置
    outWorldPosition = ubo_instance.model * vec4(inPosition, 1.0);
}
