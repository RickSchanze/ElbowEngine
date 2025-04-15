module Math;

public static const float PI = 3.141592653589793;

public float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
public float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

public float2 GetUVFromDirection(float3 dir) {
    float3 normilized_dir = normalize(dir);
    float phi = atan2(dir.z, dir.x) + PI;
    float theta = acos(dir.y);
    float2 uv;
    uv.x = 1 - phi / (2.f * PI);
    uv.y = theta / PI;
    return uv;
}

public float3 GetNormalFromUV(float2 uv) {
    float phi = (uv.x) * 2 * PI;
    float theta = uv.y * PI;
    return float3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
}


