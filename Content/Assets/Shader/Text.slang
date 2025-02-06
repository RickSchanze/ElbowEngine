/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex_UI
 * @Blend: On
 */

import CommonStructure;
import Utility;
import Annotations;

public struct FontParams {
  public float4 color;
}

uniform ConstantBuffer<Camera> camera;
uniform Texture2D font_atlas;
uniform SamplerState common_sampler;
uniform ConstantBuffer<FontParams> font_params;

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
  float sampled_color = font_atlas.Sample(common_sampler, input.uv).r;
  output.color = float4(1, 0, 0, sampled_color);
  return output;
}
