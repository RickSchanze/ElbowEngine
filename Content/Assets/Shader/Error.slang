/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 */

import CommonStructure;
#include "Common/InputUniform.slang"

    import CommonStructure;
    uniform ConstantBuffer<VP> vp;
    uniform ConstantBuffer<M> m;

struct VertexOutput {
    float4 position: SV_Position;
}

struct FragOutput {
    float4 color: SV_Target;
}

[shader("vertex")]
VertexOutput vert(Vertex vert) {
    VertexOutput output;
    float4x4 mvp = mul(m.model, mul(vp.view, vp.projection));
    output.position = mul(float4(vert.position, 1), mvp);
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    FragOutput output;
    output.color = float4(1, 0, 1, 1);
    return output;
}