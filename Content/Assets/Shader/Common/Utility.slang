module Utility;

public float4 PerformMVP(float4 pos, float4x4 m, float4x4 v, float4x4 p) {
    float4 m = mul(m, pos);
    float4 v = mul(v, m);
    float4 p = mul(p, v);
    return p;
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