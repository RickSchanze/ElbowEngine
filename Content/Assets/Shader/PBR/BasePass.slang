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

struct MatrixParam {
    float4x4 normal_matrix;
}

uniform ConstantBuffer<Camera> camera;
uniform ConstantBuffer<GlobalLights> dynamic_lights;

uniform ConstantBuffer<PBRFloatParam> float_param;
uniform ConstantBuffer<MatrixParam> martix_param;

uniform Texture2D tex;
uniform Texture2D<float> tex_metallic;
uniform Texture2D<float> tex_roughness;
uniform Texture2D<float> tex_ao;
uniform Texture2D<float3> tex_normal;
uniform Texture2D<float3> tex_irradiance;
uniform SamplerState common_sampler;

struct VertexOutput {
    float4 position : SV_Position;
    float3 normal;
    float3 tagent;
    float3 bitangent;
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
    output.world_position = world_pos.xyz;
    output.tagent = vert.tangent;
    output.bitangent = vert.bitangent;
    output.normal = mul(vert.normal, float3x3(model));
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    float3 camera_world_pos = camera.GetLocation();
    float3 N = input.normal;
    float3 world_pos = input.world_position;
    float3 V =  normalize(camera_world_pos - input.world_position);
    FragOutput output;

    float3 albedo = tex.Sample(common_sampler, input.uv).xyz;
    float metalness = float_param.metallic * tex_metallic.Sample(common_sampler, input.uv);
    float roughness = float_param.roughness * tex_roughness.Sample(common_sampler, input.uv);
    float ao = float_param.ao * tex_ao.Sample(common_sampler, input.uv);
    float3 F0 = GetFresnelF0(albedo.xyz, metalness);
    
    float3 L_0 = float3(0); // 渲染方程输出
    for (int i = 0; i < dynamic_lights.point_light_count; i++)
    {
        float3 L = normalize(dynamic_lights.point_light[i].location - world_pos);
        float3 H = normalize(V + L);
        float NdotV = clamp(dot(N, V), 0.0, 1.0);
        float NdotL = clamp(dot(N, L), 0.0, 1.0);
        float LdotH = clamp(dot(L, H), 0, 1);
        float NdotH = clamp(dot(N, H), 0, 1);
        float3 light_color = dynamic_lights.point_light[i].color;
        if (NdotL > 0.0) {
            float D = GGX_TrowbridgeReitz(NdotH, roughness);
            float G = GGX_GeometrySmith(NdotV, NdotL, roughness);
            float3 F = FresnelSchlick(float3(0.04), NdotV);
            float3 numerator = D * G * F;
            float3 denominator = 4 * NdotL * NdotV + 0.00001;
            float3 specular = numerator / denominator;
            
            float3 kS = F;
            float3 kD = float3(1) - kS;
            kD *= 1 - metalness;
            L_0 += (kD * albedo / PI + specular) * light_color * NdotL; 
        }
    }
    // 计算环境光
    float3 kS = FresnelSchlickRoughess(max(dot(N, V), 0.0), F0, roughness);
    float3 kD = 1 - kS;
    float3 irradiance = tex_irradiance.Sample(common_sampler, input.uv);
    float3 diffuse = irradiance * albedo.xyz; 
    float3 ambient = kD * diffuse * ao + 0.03;
    output.color = float4(L_0 + ambient, 1);
    return output;
}