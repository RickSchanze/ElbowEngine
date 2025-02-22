/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex_UI
 * @Blend: On
 * @EnableDepth: false
 */

import CommonStructure;
import Utility;
import Annotations;

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

// 单个通道的反伽马校正
float GammaToLinear(float srgb) {
  const float cutoff = 0.04045;
  const float a = 0.055;

  return (srgb <= cutoff) ? (srgb / 12.92) : pow((srgb + a) / (1.0 + a), 2.4);
}

float3 GammaToLinear(float3 srgb) {
  const float cutoff = 0.04045;
  const float a = 0.055;

  float3 linear = srgb / 12.92;
  float3 exponent = pow((srgb + a) / (1.0 + a), 2.4);
  return lerp(linear, exponent, step(cutoff, srgb));
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
  FragOutput output;
  output.color = atlas.Sample(common_sampler, input.uv);
  output.color *= input.color;
  return output;
}
