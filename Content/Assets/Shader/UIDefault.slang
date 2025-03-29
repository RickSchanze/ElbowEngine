/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex_UI
 * @Blend: On
 * @EnableDepth: false
 */

import "Common/CommonStructure";
import "Common/Utility";

uniform ConstantBuffer<Camera> camera;
uniform Texture2D atlas;
uniform SamplerState common_sampler;

struct VertexOutput {
  float4 position : SV_Position;
  float2 uv : TEXCOORD0;
  float4 color;
}

struct FragOutput {
  float4 color : SV_Target;
}

[shader("vertex")]
VertexOutput vert(VertexUI vert) {
    VertexOutput output;
    output.uv = vert.uv;
    float4 pos = mul(float4(vert.position, 1), camera.GetOrtho());
    output.position = pos;
    output.color = vert.color;
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    FragOutput output;
    output.color = atlas.Sample(common_sampler, input.uv);
    output.color *= input.color;
    return output;
}
