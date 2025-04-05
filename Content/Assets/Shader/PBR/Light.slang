module Light;

public struct PointLight {
    public float3 location;
    public float3 color; 
}

public struct GlobalLights {
    public PointLight point_light[8];
    public int point_light_count;
}
