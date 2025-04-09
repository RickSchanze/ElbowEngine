/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 * @EnableDepth: false
 * @HDR: Off
 */
// 这里给一个屏幕空间四边形, 直接采样渲染
import "Common/CommonStructure";
import "Common/Utility";
import "ToneMapping";
import "ColorCorrection";

struct HDRParam {
    float exposure; // 曝光度
}

uniform ConstantBuffer<Camera> camera;
uniform ConstantBuffer<HDRParam> param;
uniform Texture2D tex;
uniform SamplerState common_sampler;

struct VertexOutput {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
}

struct FragOutput {
    float4 color : SV_Target;
}

[shader("vertex")]
VertexOutput vert(Vertex vert) {
    VertexOutput output;
    float4 pos_vec4 = float4(vert.position, 1);
    pos_vec4.z = 0;
    pos_vec4.w = 1;
    output.position = pos_vec4;
    output.uv = vert.uv0;
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    FragOutput output;
    output.color = tex.Sample(common_sampler, input.uv);
    output.color = ReinhardToneMapping(output.color, param.exposure);
    // output.color = GammaToLinear(output.color);
    return output;
}