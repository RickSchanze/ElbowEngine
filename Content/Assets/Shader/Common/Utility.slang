module Utility;



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

