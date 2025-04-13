/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 */

import "Common/CommonStructure";
import "Common/Utility";
import "Light";
import "Fresnel";
import "GGX";
import "Common/Math";

struct PBRFloatParam {
    float metallic;
    float roughness;
    float ao;
}

struct MatrixParam {
    float4x4 NormalMatrix;
}

uniform ConstantBuffer<Camera> camera;
uniform ConstantBuffer<GlobalLights> dynamic_lights;

uniform ConstantBuffer<PBRFloatParam> float_param;
uniform ConstantBuffer<MatrixParam> InMatrixParam;

uniform Texture2D tex;
uniform Texture2D<float> tex_metallic;
uniform Texture2D<float> tex_roughness;
uniform Texture2D<float> tex_ao;
uniform Texture2D<float3> Tex_Normal;
uniform Texture2D<float3> Tex_Irradiance;
uniform Texture2D<float3> Tex_Prefiltered;
uniform Texture2D<float2> Tex_BRDFLUT;
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
    float3x3 model_3x3 = float3x3(model);

    output.position = mul(world_pos, camera.GetView());
    output.position = mul(output.position, camera.GetProjection());
    output.uv = vert.uv0;
    output.world_position = world_pos.xyz;
    output.tagent = vert.tangent;
    output.bitangent = vert.bitangent;

    float3 T = mul(vert.tangent, model_3x3);
    float3 B = mul(vert.bitangent, model_3x3);
    float3 N = mul(vert.normal, model_3x3);
    float3x3 TBN = float3x3(T.x, B.x, N.x, T.y, B.y, N.y, T.z, B.z, N.z);
    float3 TangentNormal = Tex_Normal.SampleLevel(common_sampler, vert.uv0, 0);
    output.normal = normalize(mul(TangentNormal, TBN));
    return output;
}

[shader("fragment")]
FragOutput frag(VertexOutput input) {
    float3 camera_world_pos = camera.GetLocation();
    float3 N = input.normal;
    float3 world_pos = input.world_position;
    float3 V = normalize(camera_world_pos - input.world_position);
    float3 R = reflect(-V, N);
    FragOutput output;

    float3 albedo = tex.Sample(common_sampler, input.uv).xyz;
    float Metallic = float_param.metallic * tex_metallic.Sample(common_sampler, input.uv);
    float Roughness = float_param.roughness * tex_roughness.Sample(common_sampler, input.uv);
    float AO = float_param.ao * tex_ao.Sample(common_sampler, input.uv);
    float3 F0 = GetFresnelF0(albedo.xyz, Metallic);
    
    float3 L_0 = float3(0); // 渲染方程输出
    for (int i = 0; i < dynamic_lights.point_light_count; i++)
    {
        float3 L = normalize(dynamic_lights.point_light[i].location - world_pos);
        float Distance = length(dynamic_lights.point_light[i].location - world_pos);
        float Distance2 = Distance * Distance + 0.0001;
        float3 H = normalize(V + L);
        float NdotV = clamp(dot(N, V), 0.0, 1.0);
        float NdotL = clamp(dot(N, L), 0.0, 1.0);
        float LdotH = clamp(dot(L, H), 0, 1);
        float NdotH = clamp(dot(N, H), 0, 1);
        float3 light_color = dynamic_lights.point_light[i].color * 10 / Distance2;
        if (NdotL > 0.0) {
            float D = GGX_TrowbridgeReitz(NdotH, Roughness);
            float G = GGX_GeometrySmith(NdotV, NdotL, Roughness);
            float3 F = FresnelSchlick(float3(0.04), NdotV);
            float3 numerator = D * G * F;
            float3 denominator = 4 * NdotL * NdotV + 0.00001;
            float3 specular = numerator / denominator;
            float3 kS = F;
            float3 kD = float3(1) - kS;
            kD *= 1 - Metallic;
            L_0 += (kD * albedo / PI + specular) * light_color * NdotL; 
        }
    }
    // 计算环境光
    float3 F = FresnelSchlickRoughess(max(dot(N, V), 0.0), F0, Roughness);
    float3 Ks = F;
    float3 Kd = 1 - Ks;
    Kd *= 1 - Metallic;
    float3 Irradiance = Tex_Irradiance.Sample(common_sampler, input.uv);
    float3 Diffuse = Irradiance * albedo.xyz;
    const static float MAX_REFLECTION_LOD = 4.0;
    float3 PrefilteredColor = Tex_Prefiltered.SampleLevel(common_sampler, GetUVFromDirection(R), Roughness * MAX_REFLECTION_LOD);
    float2 EnvBRDF = Tex_BRDFLUT.Sample(common_sampler, float2(max(dot(N, V), 0.0), Roughness));
    float3 Specular = PrefilteredColor * (F * EnvBRDF.x + EnvBRDF.y);
    float3 Ambient = (Kd * Diffuse + Specular) * AO;
    output.color = float4(L_0 + Ambient, 1);
    return output;
}