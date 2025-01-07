/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 */

import CommonStructure;
#include "Common/InputUniform.slang"

    import CommonStructure;

struct VertexOutput {
    float4 position: SV_Position;
}

struct FragOutput {
    float4 color: SV_Target;
}

[shader("vertex")]
VertexOutput vert(Vertex vert) {
    VertexOutput output;
    output.position = float4(vert.position, 1);
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    FragOutput output;
    output.color = float4(1, 0, 1, 1);
    return output;
}