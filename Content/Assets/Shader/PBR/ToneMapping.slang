module ToneMappings;
// ACES Filmic色调映射核心函数 (基于Narkowicz 2015优化版)
// Reinhard简易色调映射
public float4 ReinhardToneMapping(float4 color, float exposure) {
    float3 color_no_alpha = color.rgb;

    color_no_alpha *= exposure;
    color_no_alpha = color_no_alpha / (1.0 + color_no_alpha);
    return float4(color_no_alpha, color.a);
}