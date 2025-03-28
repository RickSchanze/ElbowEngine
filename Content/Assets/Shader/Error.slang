/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 */

import "Common/CommonStructure";
import "Common/Utility";

uniform ConstantBuffer<Camera> camera;

struct VertexOutput {
    float4 position: SV_Position;
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
    output.color = float4(1, 0, 1, 1);
    return output;
}