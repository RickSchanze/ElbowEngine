/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 */

import "Common/CommonStructure";
import "Common/Utility";
import "Light";
import "Fresnel";
import "GGX";

struct PBRFloatParam {

    float metallic;
    float roughness;
    float ao;
}

uniform ConstantBuffer<Camera> camera;
uniform ConstantBuffer<GlobalLights> dynamic_lights;

uniform ConstantBuffer<PBRFloatParam> float_param;

uniform Texture2D tex;
uniform Texture2D<float> tex_metallic;
uniform Texture2D<float> tex_roughness;
uniform Texture2D<float> tex_ao;
uniform SamplerState common_sampler;

struct VertexOutput {
    float4 position : SV_Position;
    float3 normal;
    float3 world_position;
    float2 uv : TEXCOORD0;
}

struct FragOutput {
    float4 color: SV_Target;
}

[shader("vertex")]
VertexOutput vert(Vertex vert) {
    VertexOutput output;
    float4 pos_vec4 = float4(vert.position, 1);
    float4x4 model = vert.GetModelFloat4x4();
    float4 world_pos = mul(pos_vec4, model);
    output.position = mul(world_pos, camera.GetView());
    output.position = mul(output.position, camera.GetProjection());
    output.uv = vert.uv0;
    output.normal = vert.normal;
    output.world_position = world_pos.xyz;
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    float3 camera_world_pos = camera.GetLocation();
    float3 N = normalize(input.normal);
    float3 world_pos = input.world_position;
    float3 V = normalize(camera_world_pos - input.world_position);
    FragOutput output;

    float4 albedo = tex.Sample(common_sampler, input.uv);
    float metalness = float_param.metallic * tex_metallic.Sample(common_sampler, input.uv);
    float roughness = float_param.roughness * tex_roughness.Sample(common_sampler, input.uv);
    float ao = float_param.ao * tex_ao.Sample(common_sampler, input.uv);
    float3 L_0 = float3(0); // 渲染方程输出
    for (int i = 0; i < dynamic_lights.point_light_count; i++)
    {
        float3 temp = dynamic_lights.point_light[i].location - world_pos; 
        float distance = length(temp);
        float3 L = normalize(temp); // 光照方向
        float3 H = normalize(L + V);
        float attenuation = 1.f / (distance * distance);
        float3 radiance = dynamic_lights.point_light[i].color * attenuation;
        float3 F0 = GetFresnelF0(albedo.xyz, metalness);
        float3 F = FresnelSchlick(F0, max(dot(H, V), 0));
        float NDF = GGX_TrowbridgeReitz(N, H, roughness);
        float G = GGX_GeometrySmith(N, V, L, roughness);
        // 计算cook-torrance BRDF
        float3 numerator = NDF * G * F;
        float3 denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        float3 specular = numerator / max(denominator, 0.001);
        float3 k_S = F;
        float3 k_D = float3(1) - k_S;
        k_D *= (1 - metalness); // 金属不折射光, 没有漫反射颜色
        float NdotL = max(dot(N, L), 0.f);
        L_0 += (k_D * albedo.xyz / PI + specular) * radiance * NdotL;
    }
    float3 ambient = float3(0.03) * albedo.xyz * ao;
    output.color = float4(L_0 + ambient, albedo.a);
    return output;
}