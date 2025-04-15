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
    float Metallic;
    float Roughness;
    float AO;
}


uniform ConstantBuffer<Camera> uCamera;
uniform ConstantBuffer<GlobalLights> uDynamicLights;

uniform ConstantBuffer<PBRFloatParam> InFloatParams;

uniform Texture2D<float3> Tex_Albedo;
uniform Texture2D<float> Tex_Metallic;
uniform Texture2D<float> Tex_Roughness;
uniform Texture2D<float> Tex_AO;
uniform Texture2D<float3> Tex_Normal;
uniform Texture2D<float3> Tex_Irradiance;
uniform Texture2D<float3> Tex_Prefiltered;
uniform Texture2D<float2> Tex_BRDFLUT;
uniform SamplerState LinearSampler;

struct VertexOutput {
    float4 Position : SV_Position;
    float3 Normal;
    float3 WorldPosition;
    float2 UV0 : TEXCOORD0;
}

struct FragOutput {
    float4 Color: SV_Target;
}

[shader("vertex")]
VertexOutput Vertex(Vertex Input) {
    VertexOutput Output;
    float4 Position = float4(Input.Position, 1);
    float4x4 Model = Input.GetModelFloat4x4();
    float3 WorldPosition = mul(Position, Model).xyz;

    Output.Position = mul(mul(mul(Position, Model), uCamera.GetView()), uCamera.GetProjection());
    Output.UV0 = Input.UV0;
    Output.Normal = mul(Input.Normal, float3x3(Model)).xyz;
    Output.WorldPosition = WorldPosition;
    return Output;
}

[shader("fragment")]
FragOutput Fragment(VertexOutput Input) {
    FragOutput Output;

    float3 N = normalize(Input.Normal);
    float3 V = normalize(uCamera.GetLocation() - Input.WorldPosition);
    float3 R = reflect(-V, N);
    float NdotV = clamp(dot(N, V), 0, 1);
    float Roughess = Tex_Roughness.Sample(LinearSampler, Input.UV0) * InFloatParams.Roughness;
    float Metallic = Tex_Metallic.Sample(LinearSampler, Input.UV0) * InFloatParams.Metallic;
    float3 Albedo = Tex_Albedo.Sample(LinearSampler, Input.UV0);
    float3 L0 = float3(0.0);
    for (int I = 0; I < uDynamicLights.PointLightsCount; I++) {
        float3 L = normalize(uDynamicLights.PointLights[I].Location - Input.WorldPosition);
        float3 H = normalize(L + V);
        float Distance = length(uDynamicLights.PointLights[I].Location - Input.WorldPosition);
        float Attenuation = 1 / (Distance * Distance);
        float3 Radiance = uDynamicLights.PointLights[I].Color * Attenuation;

        float NdotH = max(0, dot(N, H));
        float HdotV = clamp(dot(H, V), 0, 1);
        float NdotL = clamp(dot(N, L), 0, 1);

        float D = GGX_Distribution(NdotH, Roughess);
        float3 F = FresnelSchlick(HdotV);
        float G = GGX_GeometrySmith(NdotV, NdotH, Roughess);

        float3 Numerator = D * F * G;
        float Denominator = 4 * NdotV * NdotL + 0.0001;
        float3 Specular = Numerator / Denominator;

        float3 Ks = F;
        float3 Kd = float3(1) - Ks;
        Kd *= 1 - Metallic;
        L0 += (Kd * Albedo / PI + Specular) * Radiance * NdotL;
    }
    float3 F = FresnelSchlickRoughess(NdotV, Roughess);
    float3 Ks = F;
    float3 Kd = 1 - Ks;
    Kd *= 1 - Metallic;
    float3 Irradiance = Tex_Irradiance.Sample(LinearSampler, Input.UV0);
    float AO = Tex_AO.Sample(LinearSampler, Input.UV0) * InFloatParams.AO;
    float3 Diffuse = Irradiance * Albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    float3 PrefilteredColor = Tex_Prefiltered.SampleLevel(LinearSampler, GetUVFromDirection(R), Roughess * MAX_REFLECTION_LOD).rgb;
    float2 BRDF = Tex_BRDFLUT.Sample(LinearSampler, float2(NdotV, Roughess));
    float3 Specular = PrefilteredColor * (F * BRDF.x + BRDF.y);

    float3 Ambient = (Kd * Diffuse + Specular) * AO;
    float3 Color = L0 + Ambient;
    Output.Color = float4(Color, 1.0);
    return Output;
}