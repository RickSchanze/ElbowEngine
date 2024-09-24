#version 450

// Shared set between most vertex shaders
layout(set = 0, binding = 0) uniform ViewUniforms {
    mat4 proj;
    mat4 view;
    mat4 camera;
} ubo_view;

layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;
layout(location = 2) out vec3 cameraPos;
layout(location = 3) out vec3 clearColor;

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[] (
vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(vec3 vec, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectPoint = viewInv * projInv * vec4(vec, 1.0);
    return unprojectPoint.xyz / unprojectPoint.w;
}


// normal vertice projection
void main() {
    vec3 p = gridPlane[gl_VertexIndex].xyz;
    // 将点反投影到无穷近和无穷远
    nearPoint = UnprojectPoint(vec3(p.xy, 0), ubo_view.view, ubo_view.proj);
    farPoint = UnprojectPoint(vec3(p.xy, 1), ubo_view.view, ubo_view.proj);
    gl_Position = vec4(p, 1.0);
    cameraPos = vec3(ubo_view.camera[0][0], ubo_view.camera[0][1], ubo_view.camera[0][2]);
    clearColor = vec3(0.5, 0.5, 0.5);
}
