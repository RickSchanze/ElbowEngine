module Light;

public struct PointLight {
    public float3 Location;
    public float3 Color; 
}

public struct GlobalLights {
    public PointLight PointLights[8];
    public int PointLightsCount;
}
