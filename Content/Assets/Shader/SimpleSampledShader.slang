/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 */

import CommonStructure;
import Utility;
import Annotations;

struct TestParam {
    [Label("测试参数")]
    float3 test_param;
};

uniform ConstantBuffer<Camera> camera;

uniform Texture2D tex;
uniform SamplerState common_sampler;
uniform ConstantBuffer<TestParam> other_param;

struct VertexOutput {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
}

struct FragOutput {
    float4 color: SV_Target;
}

[shader("vertex")]
VertexOutput vert(Vertex vert) {
    VertexOutput output;
    float4 pos_vec4 = float4(vert.position, 1);
    output.position = PerformMVP(pos_vec4, vert.GetModelFloat4x4(), camera.GetView(), camera.GetProjection());
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    FragOutput output;
    output.color = tex.Sample(common_sampler, input.uv);
    return output;
}