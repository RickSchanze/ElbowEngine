/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex_UI
 * @Blend: On
 */

import CommonStructure;
import Utility;
import Annotations;

struct PanelParams {
  float4 background_color;
}

uniform ConstantBuffer<Camera> camera;
uniform Texture2D background_img;
uniform SamplerState common_sampler;
uniform ConstantBuffer<PanelParams> panel_params;

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
  float4 background = background_img.Sample(common_sampler, input.uv);
  output.color = background * panel_params.background_color;  
  return output;
}
