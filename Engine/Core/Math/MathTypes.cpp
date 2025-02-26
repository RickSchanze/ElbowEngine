/**
 * @file MathTypes.cpp
 * @author Echo
 * @Date 24-5-9
 * @brief
 */

#include "MathTypes.h"

#include "Math.h"

#include <format>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/Reflection.h"
#include <glm/trigonometric.hpp>

#include GEN_HEADER("Core.MathTypes.generated.h")

GENERATED_SOURCE()

using namespace core;

// 注册glm::vec3 glm::vec4 glm::quat
static Type *REFL_Register_Vec3() {
  using namespace core;
  Type *type = Type::Create<glm::vec3>("core.Vector3")->SetAttribute(Type::Trivial);
  type->Internal_RegisterField("x", &glm::vec3::x, offsetof(glm::vec3, x));
  type->Internal_RegisterField("y", &glm::vec3::y, offsetof(glm::vec3, y));
  type->Internal_RegisterField("z", &glm::vec3::z, offsetof(glm::vec3, z));
  return type;
}

static Type *REFL_Register_Vec4() {
  using namespace core;
  Type *type = Type::Create<glm::vec4>("core.Vector4")->SetAttribute(Type::Trivial);
  type->Internal_RegisterField("x", &glm::vec4::x, offsetof(glm::vec4, x));
  type->Internal_RegisterField("y", &glm::vec4::y, offsetof(glm::vec4, y));
  type->Internal_RegisterField("z", &glm::vec4::z, offsetof(glm::vec4, z));
  type->Internal_RegisterField("w", &glm::vec4::w, offsetof(glm::vec4, w));
  return type;
}

static Type *REFL_Register_Quat() {
  using namespace core;
  Type *type = Type::Create<glm::quat>("core.Quaternion")->SetAttribute(Type::Trivial);
  type->Internal_RegisterField("x", &glm::quat::x, offsetof(glm::quat, x));
  type->Internal_RegisterField("y", &glm::quat::y, offsetof(glm::quat, y));
  type->Internal_RegisterField("z", &glm::quat::z, offsetof(glm::quat, z));
  type->Internal_RegisterField("w", &glm::quat::w, offsetof(glm::quat, w));
  return type;
}

static Type *REFL_Register_Vec2I() {
  using namespace core;
  Type *type = Type::Create<Vector2I>("core.Vector2I")->SetAttribute(Type::Trivial);
  type->Internal_RegisterField("x", &Vector2I::x, offsetof(Vector2I, x));
  type->Internal_RegisterField("y", &Vector2I::y, offsetof(Vector2I, y));
  return type;
}

static void REFL_ConstructVec3(void *ptr) { new (ptr) glm::vec3(); }

static void REFL_ConstructVec4(void *ptr) { new (ptr) glm::vec4(); }

static void REFL_ConstructQuat(void *ptr) { new (ptr) glm::quat(); }

static void REFL_ConstructVec2I(void *ptr) { new (ptr) Vector2I(); }

static void REFL_DestroyDummy(void *ptr) {}

void Z_MetaInfo_Registration_Func1() {
  using namespace core;
  MetaInfoManager::Get()->RegisterTypeRegisterer(RTTITypeInfo::Create<glm::vec3>(), &REFL_Register_Vec3);
  CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<glm::vec3>(), &REFL_ConstructVec3, &REFL_DestroyDummy);

  MetaInfoManager::Get()->RegisterTypeRegisterer(RTTITypeInfo::Create<glm::vec4>(), &REFL_Register_Vec4);
  CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<glm::vec4>(), &REFL_ConstructVec4, &REFL_DestroyDummy);

  MetaInfoManager::Get()->RegisterTypeRegisterer(RTTITypeInfo::Create<glm::quat>(), &REFL_Register_Quat);
  CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<glm::quat>(), &REFL_ConstructQuat, &REFL_DestroyDummy);

  MetaInfoManager::Get()->RegisterTypeRegisterer(RTTITypeInfo::Create<Vector2I>(), &REFL_Register_Vec2I);
  CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<Vector2I>(), &REFL_ConstructVec2I, &REFL_DestroyDummy);
}

namespace {
struct Z_MetaInfo_Registration1 {
  Z_MetaInfo_Registration1() { Z_MetaInfo_Registration_Func1(); }
};
static const Z_MetaInfo_Registration1 Z_meta_info_registration1;
} // namespace

namespace core {

String Size2D::ToString() const { return std::format("Width: {}, Height:{}", width, height); }

Vector2::operator glm::vec<2, float>() const { return {x, y}; }

Vector2 Vector2::operator+(const Vector2 &other) const { return {x + other.x, y + other.y}; }

Vector2 Vector2::operator*=(const float scalar) const { return Vector2(x * scalar, y * scalar); }

Vector2 Vector2::operator*(float x) const { return {this->x * x, y * x}; }

Vector2 Vector2::operator/(float scalar) const { return {x / scalar, y / scalar}; }

Vector2 Vector2::operator-(const Float other) const { return {x - other, y - other}; }
Vector2 &Vector2::operator+=(const Float other) {
  x += other;
  y += other;
  return *this;
}

Vector2 operator-(Float s, Vector2 v) { return {s - v.x, s - v.y}; }
Vector2 operator-(Vector2 v1, Vector2 v2) { return {v1.x - v2.x, v1.y - v2.y}; }

Vector3 Rotator::GetForwardVector() const {
  Vector3 Forward;
  Forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  Forward.y = sin(glm::radians(pitch));
  Forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  return normalize(Forward);
}

Vector3 Rotator::GetUpVector() const { return cross(GetRightVector(), GetForwardVector()); }

Vector3 Rotator::GetRightVector() const { return cross(GetForwardVector(), Constant::UpVector); }

String Rotator::ToString() const { return std::format("Yaw: {}, Pitch: {}, Roll: {}", yaw, pitch, roll); }

bool Rotator::operator==(const Rotator &other) const {
  return Math::ApproximatelyEqual(yaw, other.yaw) && Math::ApproximatelyEqual(pitch, other.pitch) &&
         Math::ApproximatelyEqual(roll, other.roll);
}

bool Rotator::operator!=(const Rotator &other) const { return !(*this == other); }

Rotator &Rotator::operator=(const Vector3 &v) {
  yaw = v.x;
  pitch = v.y;
  roll = v.z;
  return *this;
}

Rotator Rotator::operator-(const Rotator &other) const {
  Rotator rtn;
  rtn.yaw = Math::RoundAngle(yaw - other.yaw, -180.0f, 180.0f);
  rtn.pitch = Math::RoundAngle(pitch - other.pitch, -180.0f, 180.0f);
  rtn.roll = Math::RoundAngle(roll - other.roll, -180.0f, 180.0f);
  return rtn;
}

Rotator Rotator::operator+(const Rotator &other) const {
  Rotator rtn;
  rtn.yaw = Math::RoundAngle(yaw + other.yaw, -180.0f, 180.0f);
  rtn.pitch = Math::RoundAngle(pitch + other.pitch, -180.0f, 180.0f);
  rtn.roll = Math::RoundAngle(roll + other.roll, -180.0f, 180.0f);
  return rtn;
}

Rotator &Rotator::operator+=(const Rotator &other) {
  yaw = Math::RoundAngle(yaw + other.yaw);
  pitch = Math::RoundAngle(pitch + other.pitch);
  roll = Math::RoundAngle(roll + other.roll);
  return *this;
}

Rotator &Rotator::operator-=(const Rotator &other) {
  yaw = Math::RoundAngle(yaw - other.yaw);
  pitch = Math::RoundAngle(pitch - other.pitch);
  roll = Math::RoundAngle(roll - other.roll);
  return *this;
}

Rotator Rotator::ToRotatorRadian() const {
  Rotator r;
  r.roll = Math::Radians(roll);
  r.pitch = Math::Radians(pitch);
  r.yaw = Math::Radians(yaw);
  return r;
}

uint8_t Color::FloatToByte(float value) { return static_cast<uint8_t>(std::round(value * 255.0f)); }

bool Color::IsValid() const { return r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1 && a >= 0 && a <= 1; }

Color Color::operator*(const Color &other) const { return {r * other.r, g * other.g, b * other.b, a * other.a}; }

Color Color::operator*(const float scalar) const { return {r * scalar, g * scalar, b * scalar, a * scalar}; }

uint32_t Color::ToUInt() const {
  return FloatToByte(a) << 24 | FloatToByte(r) << 16 | FloatToByte(g) << 8 | FloatToByte(b);
}

uint32_t Color::ToUIntNoAlpha() const { return FloatToByte(r) << 16 | FloatToByte(g) << 8 | FloatToByte(b); }

bool Color::operator==(const Color &other) const {
  return Math::ApproximatelyEqual(r, other.r) && Math::ApproximatelyEqual(g, other.g) &&
         Math::ApproximatelyEqual(b, other.b) && Math::ApproximatelyEqual(a, other.a);
}

bool Color::operator!=(const Color &other) const { return !(*this == other); }

String Rect2D::ToString() const {
  return String::Format("Rect[position={{{}, {}}}, size={{{}, {}}}]", position.x, position.y, size.x, size.y);
}

Vector2 Rect2D::Center() const { return Vector2(position.x + size.x / 2.0f, position.y + size.y / 2.0f); }

Vector2 Rect2D::LeftTop() const { return {position.x, position.y + size.y}; }

Vector2 Rect2D::RightTop() const { return position + size; }

Vector2 Rect2D::LeftBottom() const { return position; }

Vector2 Rect2D::RightBottom() const { return {position.x + size.x, position.y}; }

Size2D Rect2D::Size() const { return Size2D(size.x, size.y); }

Float Rect2D::Area() const { return size.x * size.y; }

Rect2D Rect2D::operator/(const Float scalar) const { return {position / scalar, size / scalar}; }

Vector2I Rect2DI::LeftTop() const { return Vector2I(position.x, position.y + size.y); }
Vector2I Rect2DI::RightTop() const { return Vector2I(position.x + size.x, position.y + size.y); }
Vector2I Rect2DI::LeftBottom() const { return Vector2I(position.x, position.y); }
Vector2I Rect2DI::RightBottom() const { return Vector2I(position.x + size.x, position.y); }

Rect2D operator/(Rect2DI rect, Vector2I size) {
  Rect2D rtn;
  rtn.position.x = (Float)rect.position.x / size.x;
  rtn.position.y = (Float)rect.position.y / size.y;
  rtn.size.x = (Float)rect.size.x / size.x;
  rtn.size.y = (Float)rect.size.y / size.y;
  return rtn;
}

} // namespace core
