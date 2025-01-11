module CommonStructure;

public struct Vertex {
    public float3 position;
    public float3 normal;
    public float2 uv0;
};

public struct Camera {
    float4x4 view;
    float4x4 projection;
    float4x4 data1;
    float4x4 data2;

    public float4x4 GetView() {
        return view;
    }

    public float4x4 GetProjection() {
        return projection;
    }

    public float3 GetPosition() {
        return data1[0].xyz;
    }
};

public struct Model {
    float4x4 model;

    public float4x4 GetModel() {
        return model;
    }
}