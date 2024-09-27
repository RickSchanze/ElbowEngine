#version 450

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 nearPoint;
layout (location = 1) in vec3 farPoint;
layout (location = 2) in vec3 cameraPos;
layout (location = 3) in vec3 clearColor;
layout (location = 4) in mat4 proj;
layout (location = 8) in mat4 view;

// TODO: 优化： 抗锯齿

float normal_grid_size = 10;
float big_grid_size_scale = 10;  // 10个小格子组成一个大格子
float layer_height = 400;        // 摄像机每抬高/降低这个高度，则把小格子合并成大格子，原先的大格子变成小格子，原先的小格子消失
float line_width = 0.5f;        // 线宽
float fade_begin = 1000.f;
float fade_end = 1500.f;
vec4 x_color = vec4(1, 0, 0, 1); // x轴颜色
vec4 z_color = vec4(0, 0, 1, 1); // z轴颜色
vec4 line_color = vec4(0, 0, 0, 1);

vec2 CalcLayer(float height) {
    float layer1 = layer_height;
    float layer2 = layer1 * big_grid_size_scale;
    float layer3 = layer2 * big_grid_size_scale;
    float layer4 = layer3 * big_grid_size_scale;
    if (height < layer1) {
        return vec2(0, height / layer1);
    } else if (height < layer2) {
        return vec2(1, (height - layer1) / (layer2 - layer1));
    } else if (height < layer3) {
        return vec2(2, (height - layer2) / (layer3 - layer2));
    } else {
        return vec2(3, 0);
    }
}

vec4 EaseIn(vec4 a, vec4 b, float t) {
    float t1 = t * t;
    return mix(a, b, t1);
}

float CalcDepth(vec3 pos) {
    vec4 clip_space_pos = proj * view * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

void main() {
    float t = (-nearPoint / (farPoint - nearPoint)).y;
    if (t >= 0) {
        float camera_y = cameraPos.y;
        vec3 zero_point = nearPoint + t * (farPoint - nearPoint);
        vec2 zero_xz = zero_point.xz;
        float big_grid_size = normal_grid_size * big_grid_size_scale;

        vec2 layer = CalcLayer(camera_y);
        float num_layer = layer.x;
        float alpha = layer.y;
        line_width = line_width * (num_layer + 1);

        normal_grid_size = normal_grid_size * pow(big_grid_size_scale, num_layer);
        vec2 dist = mod(abs(zero_xz), vec2(normal_grid_size, normal_grid_size));
        float dist_min = min(dist.x, dist.y);
        big_grid_size = normal_grid_size * big_grid_size_scale;
        vec2 big_dist = mod(abs(zero_xz) , vec2(big_grid_size, big_grid_size));
        float big_dist_min = min(big_dist.x, big_dist.y);

        bool on_x = bool(abs(zero_xz.x) <= (line_width / 2));
        bool on_z = bool(abs(zero_xz.y) <= (line_width / 2));
        bool on_big_grid = bool(abs(big_dist_min) < (line_width / 2));
        bool on_grid = bool(abs(dist_min) < (line_width / 2));
        gl_FragDepth = CalcDepth(zero_point);

        float dist_to_cam = distance(cameraPos, zero_point);
        float fade_out_alpha = (dist_to_cam - fade_begin) / (fade_end - fade_begin);
        fade_out_alpha = clamp(fade_out_alpha, 0, 1);
        x_color = vec4(0.5, 0.5, 0.5, 1) * fade_out_alpha + x_color * (1 - fade_out_alpha);
        z_color = vec4(0.5, 0.5, 0.5, 1) * fade_out_alpha + z_color * (1 - fade_out_alpha);
        line_color = vec4(0.5, 0.5, 0.5, 1) * fade_out_alpha + line_color * (1 - fade_out_alpha);

        if (on_x) {
            outColor = x_color;
        } else if (on_z) {
            outColor = z_color;
        } else if (on_big_grid) {
            outColor = line_color;
        } else if (on_grid) {
            outColor = vec4(0.5, 0.5, 0.5, 1) * alpha + line_color * (1 - alpha);
        } else {
            discard;
        }
    } else {
        discard;
    }
}
