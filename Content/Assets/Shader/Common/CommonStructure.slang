module CommonStructure;

import Utility;

public struct Vertex {
  public float3 position;
  public float3 normal;
  public float2 uv0;

  // 实例化数据: 整个模型的位置, 旋转, 缩放
  public float3 location;
  public float3 rotation;
  public float3 scale;

  public float4x4 GetModelFloat4x4() {
    float4x4 scale = float4x4(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
    float4x4 rot = GetFloat4x4Identity();
    rot = RotateX(rotation[0], rot);
    rot = RotateY(rotation[1], rot);
    rot = RotateZ(rotation[2], rot);
    float4x4 trans = GetFloat4x4Identity();
    trans = Translate(location, trans);
    return mul(scale, mul(rot, trans));
  }
};

public struct VertexUI {
  public float3 position;
  public float2 uv;
  public float4 color;
}

public struct Camera {
  float4x4 view;
  float4x4 projection;
  float4x4 data1;
  float4x4 data2;

  public float4x4 GetView() { return view; }

  public float4x4 GetProjection() { return projection; }

  public float3 GetPosition() { return data1[0].xyz; }
};

public struct Model {
  float4x4 model;

  public float4x4 GetModel() { return model; }
}

public struct Texture2DSampler {
  Texture2D texture;
  SamplerState sampler;

  public float4 Sample(float2 uv) { return texture.Sample(sampler, uv); }
}
