import "Common/TestModule";

struct VertexInput {
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VertexOutput {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct FragmentOutput {
    float4 color : SV_TARGET;
};

struct MVP {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

uniform MVP mvp;

struct Color : IColor {
    float4 color;

    float4 GetColor() {
        return float4(1, 1, 1, 1);
        }

}

[shader("vertex")]
VertexOutput vertex(VertexInput input) {
    VertexOutput output;
    output.position = mul(mvp.projection, mul(mvp.view, mul(mvp.model, input.position)));
    output.normal = normalize(mul(float4(input.normal, 0.0), mvp.model).xyz);
    output.uv = input.uv;
    return output;
}

[shader("fragment")]
FragmentOutput fragment(VertexOutput input) {
    FragmentOutput output;
    IColor col;
    output.color = col.GetColor();
    return output;
}
