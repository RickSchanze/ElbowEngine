/**
 * @Pipeline: Graphics
 * @InputLayout: Vertex1
 * @HDR: Off
 */

import "Common/CommonStructure";
import "Common/Utility";
import "Light";
import "Fresnel";
import "GGX";
import "Common/Math";

struct MatrixParam {
    float4x4 Projection;
    float4x4 Model;
}

struct PushConstantParam {
    float4x4 View;
}

struct Float3Param {
    float3 LightPos;
}

struct VertexOutput {
    float4 Position : SV_Position;
    float4 WorldPosition;
    float3 LightPos;
}

ConstantBuffer<MatrixParam> uMatrix;
ConstantBuffer<Float3Param> uFloat;

[[vk::push_constant]]
ConstantBuffer<PushConstantParam> uPush;

[shader("vertex")]
VertexOutput Vertex(Vertex Input) {
    VertexOutput Output;
    Output.WorldPosition = mul(float4(Input.Position, 1), Input.GetModelFloat4x4());
    Output.Position = mul(mul(mul(Output.WorldPosition, uMatrix.Model), uPush.View), uMatrix.Projection);
    Output.LightPos = uFloat.LightPos;
    return Output;
}

struct FrgmentOutput {
    float Depth : SV_Target;
}

[shader("fragment")]
FrgmentOutput Fragment(VertexOutput Input) {
    float3 LightVec = Input.WorldPosition.xyz - Input.LightPos;
    FrgmentOutput Output;
    Output.Depth = length(LightVec);
    return Output;
}