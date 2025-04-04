module ColorCorrection;

public float4 GammaToLinear(float4 srgb) {
    const float cutoff = 0.04045;
    const float a = 0.055;
    float3 srgb_no_a = srgb.rgb;
    float3 linear = srgb_no_a / 12.92;
    float3 exponent = pow((srgb_no_a + a) / (1.0 + a), 2.4);
    float3 result_no_a = lerp(linear, exponent, step(cutoff, srgb_no_a));
    return float4(result_no_a, srgb.a);
}