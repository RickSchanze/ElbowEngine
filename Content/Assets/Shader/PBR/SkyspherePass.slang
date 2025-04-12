/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 * @CullMode: Front
 */

import "Common/CommonStructure";
import "Common/Utility";

uniform ConstantBuffer<Camera> camera;
uniform Texture2D skybox_texture;
uniform SamplerState common_sampler;

struct VertOutput {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float3 ray_dir;
}

struct FragOutput {
    float4 color : SV_Target;
}

[shader("vertex")]
VertOutput vert(Vertex vert) {
    float4 vert_pos = float4(vert.position, 1);
    float3 cam_loc = camera.GetLocation();
    float4x4 model = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        cam_loc.x, cam_loc.y, cam_loc.z, 1,
    };

    float4 outpos = PerformMVP(vert_pos, model, camera.GetView(), camera.GetProjection());
    outpos.z = outpos.w;
    VertOutput output;
    output.position = outpos;
    output.ray_dir = vert.position;
    return output;
}

[shader("fragment")]
FragOutput frag(VertOutput vert_in) {
    float2 uv = GetUVFromDirection(vert_in.ray_dir);
    FragOutput output;
    output.color = skybox_texture.SampleLevel(common_sampler, uv, 1);
    return output;
}

 