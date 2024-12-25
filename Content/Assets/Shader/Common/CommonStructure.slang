module CommonStructure;

public struct Vertex {
    public float3 position;
    public float3 normal;
    public float2 uv0;
};

public struct VP {
    public float4x4 view;
    public float4x4 projection;
};

public struct M {
    public float4x4 model;
};

public struct Camera {
    float4x4 data;

    public float3 GetPosition() {
        return data[0].xyz;
    }
};