module GGX;
import "Common/Utility";

// 这个GGX一版用于发现分布函数
// 直接传入粗糙度
public float GGX_TrowbridgeReitz(float3 normal, float3 half, float roughness) {
    // According to Epic, 用平方更正确一点
    float a = roughness * roughness; // 这里对粗糙度二次加工
    float a2 = a * a;
    float NdotH = max(dot(normal, half), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1);
    denom = denom * denom * PI;

    return nom / denom;
}

public float GGX_Schlick(float NdotV, float roughness) {
    // 这里只是直接光照的计算方法
    // According to Epic, 用平方更正确一点
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nom = NdotV;
    float denom = NdotV * (1 - k) + k;
    return nom / denom;
}

/// N: 法线
/// V: 实现方向
/// L: 光纤方向
/// 这里为了考虑两个视角, 一个是视线方向造成的遮挡, 一个是光源方向造成的阴影
public float GGX_GeometrySmith(float3 N, float3 V, float3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GGX_Schlick(NdotV, roughness);
    float ggx2 = GGX_Schlick(NdotL, roughness);
    return ggx1 * ggx2;
}
