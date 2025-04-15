module CommonStructure;

import Utility;

public struct Vertex {
  public float3 Position;
  public float3 Normal;
  public float2 UV0;
  public float3 Tangent;
  

  // 实例化数据: 整个模型的位置, 旋转, 缩放
  public float3 Location;
  public float3 Rotation;
  public float3 Scale;

  public float4x4 GetModelFloat4x4() {
    float3 Euler = Rotation; // pitch(y), yaw(z), roll(x)
    float CZ = cos(Euler.z), SZ = sin(Euler.z);
    float CY = cos(Euler.y), SY = sin(Euler.y);
    float CX = cos(Euler.x), SX = sin(Euler.x);

        // 绕Z轴旋转矩阵
    float4x4 RZ = {
        float4(CZ, -SZ, 0, 0),
        float4(SZ, CZ, 0, 0),
        float4(0, 0, 1, 0),
        float4(0, 0, 0, 1)
    };  

      // 绕Y轴旋转矩阵
    float4x4 RY = {
      float4(CY, 0, SY, 0),
      float4(0, 1, 0, 0),
      float4(-SY, 0, CY, 0),
      float4(0, 0, 0, 1)
    };

      // 绕X轴旋转矩阵
    float4x4 RX = {
      float4(1, 0, 0, 0),
      float4(0, CX, -SX, 0),
      float4(0, SX, CX, 0),
      float4(0, 0, 0, 1)
    };

    float4x4 R = mul(mul(RZ, RY), RX); // Z-Y-X顺序组合

    float4x4 T = {
      float4(1, 0, 0, 0),
      float4(0, 1, 0, 0),
      float4(0, 0, 1, 0),
      float4(Location, 1)
    };
    float4x4 S = {
        float4(Scale.x, 0, 0, 0),
        float4(0, Scale.y, 0, 0),
        float4(0, 0, Scale.z, 0),
        float4(0, 0, 0, 1)
    };
    return mul(T, mul(R, S));
  }
};

public struct Camera {
  float4x4 view;
  float4x4 projection;
  float4x4 ortho;
  float4x4 data2;

  public float4x4 GetView() { return view; }

  public float4x4 GetProjection() { return projection; }

  public float4x4 GetOrtho() { return ortho; }

  public float3 GetLocation() {
      return float3(data2[0][1], data2[1][1], data2[2][1]);
  }

  public float3 GetRotation() {
      return float3(data2[0][2], data2[1][2], data2[2][2]);
  }
};
