module Utility;

public static const float PI = 3.141592653589793;

public float4 PerformMVP(float4 pos, float4x4 m, float4x4 v, float4x4 p) {
    float4 m = mul(pos, m);
    float4 v = mul(m, v);
    float4 p1 = mul(v, p);
    return p1;
}

public float4 PerformP(float4 pos, float4x4 p) {
    float4 result = mul(pos, p);
    return result;
}

/**
 * Returns a float4x4 identity matrix.
 */
public float4x4 GetFloat4x4Identity() {
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

public float4x4 RotateX(float angle, float4x4 m) {
    float cx = cos(angle);
    float sx = sin(angle);
    float4x4 rotation_matrix = float4x4(
        1, 0, 0, 0,
        0, cx, -sx, 0,
        0, sx, cx, 0,
        0, 0, 0, 1
    );
    return mul(m, rotation_matrix);
}

public float4x4 RotateY(float angle, float4x4 m) {
    float cy = cos(angle);
    float sy = sin(angle);
    float4x4 rotation_matrix = float4x4(
        cy, 0, sy, 0,
        0, 1, 0, 0,
        -sy, 0, cy, 0,
        0, 0, 0, 1
    );
    return mul(m, rotation_matrix);
}

public float4x4 RotateZ(float angle, float4x4 m) {
    float cz = cos(angle);
    float sz = sin(angle);
    float4x4 rotation_matrix = float4x4(
        cz, -sz, 0, 0,
        sz, cz, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
    return mul(m, rotation_matrix);
}

public float4x4 Translate(float x, float y, float z, float4x4 m) {
    float4x4 translation_matrix = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, -z, 1
    );
    return mul(m, translation_matrix);
}

public float4x4 Translate(float3 translation, float4x4 m) {
    return Translate(translation.x, translation.y, translation.z, m);
}

public float2 GetUVFromDirection(float3 dir) {
    float3 normilized_dir = normalize(dir);
    float phi = atan2(dir.z, dir.x) + PI;
    float theta = acos(dir.y);
    float2 uv;
    uv.x = phi / (2.f * PI);
    uv.y = theta / PI;
    return uv;
}

public float3 GetNormalFromUV(float2 uv) {
    float phi = uv.x * 2 * PI;
    float theta = uv.y * PI;
    return float3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
}
