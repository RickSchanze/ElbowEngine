module CommonStructure;

public struct Vertex {
    public float3 position;
    public float3 normal;
    public float2 uv0;
};

public struct Camera {
    float4x4 data;
    float4x4 view;
    float4x4 projection;

    public float4x4 GetView() {
        return view;
    }

    public float4x4 GetProjection() {
        return projection;
    }

    public float3 GetPosition() {
        return data[0].xyz;
    }
};