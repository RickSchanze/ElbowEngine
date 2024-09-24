#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 nearPoint;
layout(location = 1) in vec3 farPoint;
layout(location = 2) in vec3 cameraPos;
layout(location = 3) in vec3 clearColor;

const float base_grid_size = 5.f;
const float base_highlight_grid_size = 50.f;
const float axis_line_width = 0.1f;
const float highlight_line_width = 0.01f;
const float line_width = 0.05f;
const float fade_start = 10.f;
const float fade_end = 200.f;
const vec3 normal_line_color = vec3(1, 1, 0);

void main() {
    float t = (-nearPoint / (farPoint - nearPoint)).y;
    if (t >= 0) {
        vec3 zero_point = nearPoint + t * (farPoint - nearPoint);
        float distance_camera = distance(zero_point, cameraPos);
        // 动态调整网格大小
        float grid_size = base_grid_size;
        float highlight_grid_size = base_highlight_grid_size;
        // 计算网格位置
        vec2 zero_point_xz = zero_point.xz;
        vec2 grid_pos = zero_point_xz / grid_size;
        vec2 grid_cell = fract(grid_pos);
        vec2 dist_to_line = min(grid_cell, 1 - grid_cell);
        float min_dist_to_line = min(dist_to_line.x, dist_to_line.y);
        // 高亮网格位置
        vec2 high_grid_pos = zero_point_xz / highlight_grid_size;
        vec2 high_grid_cell = fract(high_grid_pos);
        vec2 high_dist_to_line = min(high_grid_cell, 1 - high_grid_cell);
        float high_min_dist_to_line = min(high_dist_to_line.x, high_dist_to_line.y);
        // 是否在x轴或者z轴？
        bool on_x = abs(zero_point_xz.x) <= (axis_line_width / 2.0);
        bool on_z = abs(zero_point_xz.y) <= (axis_line_width / 2.0);
        bool on_high_line = abs(high_min_dist_to_line) <= (highlight_line_width / 2.0);
        bool on_line = abs(min_dist_to_line) <= (highlight_line_width / 2.0);
        if (on_x) {
            outColor = vec4(1, 0, 0, 1);
        } else if (on_z) {
            outColor = vec4(0, 0, 1, 1);
        } else if (on_high_line) {
            outColor = vec4(0, 1, 0, 1);
        } else if (on_line) {
            outColor = vec4(0, 0, 0, 0);
        } else {
            discard;
        }
    } else {
        discard;
    }
}
