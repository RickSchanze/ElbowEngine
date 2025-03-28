module ColorCorrection;

public float3 GammaToLinear(float3 srgb) {
    const float cutoff = 0.04045;
    const float a = 0.055;

    float3 linear = srgb / 12.92;
    float3 exponent = pow((srgb + a) / (1.0 + a), 2.4);
    return lerp(linear, exponent, step(cutoff, srgb));
}