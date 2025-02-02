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
  float2 uv : TEXCOORD0;
}

struct FragOutput {
  float4 color : SV_Target;
}

[shader("vertex")]
VertexOutput vert(VertexUI vert) {
  VertexOutput output;
  output.uv = vert.uv;
  return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
  FragOutput output;
  float4 sampled_color = font_atlas.Sample(common_sampler, input.uv);
  output.color = float4(font_params.color.rgb * sampled_color.rgb, font_params.color.a * sampled_color.a);
  return output;
}
