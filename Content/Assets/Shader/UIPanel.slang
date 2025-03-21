/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex_UI
 * @Blend: On
 * @EnableDepth: false
 */

import CommonStructure;
import Utility;
import Annotations;
import ColorCorrection;

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
  output.position = float4(vert.position, 1);
  output.color = vert.color;
  return output;
}



[shader("fragment")]
FragOutput frag(VertexOutput input) {
  FragOutput output;
  output.color = atlas.Sample(common_sampler, input.uv);
  output.color *= input.color;
  output.color.rgb = GammaToLinear(output.color.rgb);
  return output;
}
