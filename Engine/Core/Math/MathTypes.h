/**
 * @file MathTypes.h
 * @author Echo
 * @Date 24-5-9
 * @brief
 */

#pragma once

#include "Core/Base/EString.h"
#include "Core/CoreDef.h"
#include "Core/Reflection/MetaInfoMacro.h"
#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include GEN_HEADER("Core.MathTypes.generated.h")
#include "Core/Reflection/MetaInfoManager.h"

namespace core {
class Archive;

struct STRUCT() Size2D {
  GENERATED_STRUCT(Size2D)

  PROPERTY()
  uint32_t width;

  PROPERTY()
  uint32_t height;

  Size2D(const uint32_t w, const uint32_t h) : width(w), height(h) {}
  Size2D(const Int32 w, const Int32 h) : width(w), height(h) {}
  Size2D(const Float w, const Float h) : width(w), height(h) {}

  Size2D() = default;

  [[nodiscard]] core::String ToString() const;

  bool operator==(const Size2D &other) const { return width == other.width && height == other.height; }
};

struct STRUCT() Vector2 {
  GENERATED_STRUCT(Vector2)

  Vector2() = default;
  Vector2(float x, float y) : x(x), y(y) {}
  Vector2(Int32 x, Int32 y) : x(x), y(y) {}

  operator glm::vec2() const;
  operator glm::vec3() const { return {x, y, 0}; }
  operator glm::vec4() const { return {x, y, 0, 0}; }

  Vector2 operator+(const Vector2 &other) const;

  Vector2 operator*=(float scalar) const;
  Vector2 operator*(float x) const;
  Vector2 operator/(float scalar) const;
  Vector2 operator-(const Float other) const;

  PROPERTY()
  float x;

  PROPERTY()
  float y;
};

Vector2 operator-(Float s, Vector2 v);

typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;
typedef glm::mat4 Matrix4x4;
typedef glm::mat3 Matrix3x3;
typedef glm::quat Quaternion;
typedef glm::ivec2 Vector2I;
typedef glm::uvec2 Vector2U;
typedef glm::ivec3 Vector3I;

namespace Constant {
constexpr Vector3 ZeroVector = {0, 0, 0};
constexpr Vector3 OneVector = {1, 1, 1};
constexpr Vector3 ForwardVector = {0, 0, 1};
constexpr Vector3 BackVector = {0, 0, -1};
constexpr Vector3 RightVector = {1, 0, 0};
constexpr Vector3 LeftVector = {-1, 0, 0};
constexpr Vector3 UpVector = {0, 1, 0};
constexpr Vector3 DownVector = {0, -1, 0};

constexpr float PI = 3.14159265358979323846f;
} // namespace Constant

struct STRUCT() Rotator {
  GENERATED_STRUCT(Rotator)
public:
  PROPERTY()
  float yaw = 0;

  PROPERTY()
  float roll = 0;

  PROPERTY()
  float pitch = 0;

  Rotator(float yaw = 0, float roll = 0, float pitch = 0) : yaw(yaw), roll(roll), pitch(pitch) {}
  Rotator(const Vector3 v) : yaw(v.x), roll(v.y), pitch(v.z) {}

  [[nodiscard]] Vector3 GetForwardVector() const;
  [[nodiscard]] Vector3 GetUpVector() const;
  [[nodiscard]] Vector3 GetRightVector() const;
  [[nodiscard]] core::String ToString() const;

  bool operator==(const Rotator &) const;
  bool operator!=(const Rotator &) const;

  Rotator &operator=(const Vector3 &v);
  Rotator operator-(const Rotator &other) const;
  Rotator operator+(const Rotator &other) const;

  [[nodiscard]] Vector3 ToVector3() const { return {pitch, yaw, roll}; }

  Rotator &operator+=(const Rotator &other);
  Rotator &operator-=(const Rotator &other);

  /**
   * 将角度表示的自己转变为弧度表示
   * @return
   */
  [[nodiscard]] Rotator ToRotatorRadian() const;
};

struct STRUCT() Rect2D {
  GENERATED_STRUCT(Rect2D)
public:
  PROPERTY()
  Vector2 position = {};

  PROPERTY()
  Vector2 size = {};

  [[nodiscard]] String ToString() const;

  [[nodiscard]] Vector2 Center() const;
  [[nodiscard]] Vector2 TopLeft() const;
  [[nodiscard]] Vector2 TopRight() const;
  [[nodiscard]] Vector2 BottomLeft() const;
  [[nodiscard]] Vector2 BottomRight() const;
  /**
   * 注意！！！返回的Size2D其存储的类型是Int
   * @return
   */
  [[nodiscard]] Size2D Size() const;
  [[nodiscard]] Float Area() const;

  Rect2D operator/(const Float scalar) const;
};

struct STRUCT() Rect2DI {
  GENERATED_STRUCT(Rect2DI)
public:
  PROPERTY()
  Vector2I position = {};

  PROPERTY()
  Vector2I size = {};

  UInt64 Area() const { return size.x * size.y; }
};

struct STRUCT() Color {
  GENERATED_STRUCT(Color)
public:
  PROPERTY()
  float r = 0;

  PROPERTY()
  float g = 0;

  PROPERTY()
  float b = 0;

  PROPERTY()
  float a = 1.f;

  static Color Black() { return {0, 0, 0, 1}; }
  static Color White() { return {1, 1, 1, 1}; }
  static Color Red() { return {1, 0, 0, 1}; }
  static Color Green() { return {0, 1, 0, 1}; }
  static Color Blue() { return {0, 0, 1, 1}; }
  static Color Yellow() { return {1, 1, 0, 1}; }
  static Color Warning() { return {1, 1, 0, 1}; }
  static Color Error() { return {1, 0, 0, 1}; }
  static Color Info() { return {0, 1, 1, 1}; }
  static Color SkyBlue() { return {0.4f, 0.6f, 1.f, 1.f}; }
  static Color DefaultClear() { return {0.3f, 0.3f, 0.3f, 1.f}; }
  static Color Invalid() { return {-1, -1, -1, -1}; }

  /** 讲一个0-1的表示颜色的值转换为0-255的值 */
  static uint8_t FloatToByte(float value);

  [[nodiscard]] bool IsValid() const;

  Color operator*(const Color &other) const;
  Color operator*(float scalar) const;

  /** 转换Int带alpha通道 */
  [[nodiscard]] uint32_t ToUInt() const;
  /** 转换为没有alpha通道的Int */
  [[nodiscard]] uint32_t ToUIntNoAlpha() const;

  [[nodiscard]] Vector4 ToVector4() const { return {r, g, b, a}; }

  bool operator==(const Color &other) const;
  bool operator!=(const Color &other) const;
};

inline Matrix4x4 GetMatrix4x4Identity() {
  static auto identity = glm::mat4(1.0f);
  return identity;
}
} // namespace core
