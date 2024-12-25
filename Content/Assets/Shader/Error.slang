import CommonStructure;
#include "Common/InputUniform.slang"

DLEARE_COMMON_MVP_UNIFORM

struct VertexOuput {
    float4 position: SV_Position;
}

struct FragOutput {
    float4 color: SV_Target;
}

[shader("vertex")]
VertexOuput vert(Vertex vert) {
    VertexOuput output;
    float4x4 mvp = mul(m.model, mul(vp.view, vp.projection));
    output.position = mul(float4(vert.position, 1), mvp);
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOuput input) {
    FragOutput output;
    output.color = float4(1, 0, 1, 1);
    return output;
}