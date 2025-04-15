module Fresnel;
import "Common/Math";

// cos_theta = 表面法线n 与 观察方向n的点乘
public float3 FresnelSchlick(float CosTheta)
{
    float3 F0 = float3(0.04);
    return F0 + (1.0 - F0) * pow(clamp(1.0 - CosTheta, 0.0, 1.0), 5.0);
}

// metalness: 金属度
public float3 GetFresnelF0(float3 color, float metalness) {
    float3 F0 = float3(0.04f);
    return lerp(F0, color, metalness);
}

public float3 FresnelSchlickRoughess(float CosTheta, float Roughness) {
    float3 F0 = float3(0.04);
    return F0 + (max(float3(1 - Roughness), F0) - F0) * pow(1.0 - CosTheta, 5.0);
}

