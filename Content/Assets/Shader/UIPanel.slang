/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex_UI
 * @Blend: On
 */

import CommonStructure;
import Utility;
import Annotations;

public struct UIParams {
  public float4 color;
}

uniform Texture2D atlas;
uniform SamplerState common_sampler;
uniform ConstantBuffer<UIParams> params;

struct VertexOutput {
  float4 position : SV_Position;
  float2 uv : TEXCOORD0;
}

struct FragOutput {
  float4 color : SV_Target;
}

[shader("vertex")]
VertexOutput vert(VertexUI vert) {
  VertexOutput output;
  output.uv = vert.uv;
  output.position = float4(vert.position, 1);
  return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
  FragOutput output;
  output.color = atlas.Sample(common_sampler, input.uv);
  output.color *= params.color;
  return output;
}
