module Fresnel;
import "Common/Math";

// cos_theta = 表面法线n 与 观察方向n的点乘
public float3 FresnelSchlick(float3 F0, float cos_theta) {
    return F0 + (1.0 - F0) * pow(1 - cos_theta, 5.0f);
}

public float3 FresnelSchlick(float3 F0, float3 normal, float3 view) {
    float cos_theta = dot(normal, view);
    return F0 + (1.0 - F0) * pow(1 - cos_theta, 5.0f);
}

// metalness: 金属度
public float3 GetFresnelF0(float3 color, float metalness) {
    float3 F0 = float3(0.04f);
    return lerp(F0, color, metalness);
}

public float3 FresnelSchlickRoughess(float cos_theta, float3 F0, float roughness) {
    return F0 + (max(float3(1 - roughness), F0) - F0) * pow(1.0 - cos_theta, 5.0);
}

