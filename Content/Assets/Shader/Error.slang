/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 */

import CommonStructure;
import Utility;
import Annotations;

uniform ConstantBuffer<Camera> camera;

[DynamicUniform]
uniform ConstantBuffer<float4x4> model;

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
    output.position = PerformMVP(pos_vec4, model, camera.GetView(), camera.GetProjection());
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    FragOutput output;
    output.color = float4(1, 0, 1, 1);
    return output;
}