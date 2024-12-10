// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Size2D>(), &core::Size2D::REFLECTION_Register_Size2D_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Size2D>(), &core::Size2D::ConstructAt, &core::Size2D::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Vector2>(), &core::Vector2::REFLECTION_Register_Vector2_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Vector2>(), &core::Vector2::ConstructAt, &core::Vector2::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Rotator>(), &core::Rotator::REFLECTION_Register_Rotator_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Rotator>(), &core::Rotator::ConstructAt, &core::Rotator::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Color>(), &core::Color::REFLECTION_Register_Color_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Color>(), &core::Color::ConstructAt, &core::Color::DestroyAt); \

#undef GENERATED_BODY_IMPL_Size2D
#define GENERATED_BODY_IMPL_Size2D \
static void ConstructAt(void* ptr) { new (ptr) core::Size2D(); } \
static void DestroyAt(void* ptr) { static_cast<core::Size2D*>(ptr)->~Size2D(); } \

#undef GENERATED_BODY_IMPL_Vector2
#define GENERATED_BODY_IMPL_Vector2 \
static void ConstructAt(void* ptr) { new (ptr) core::Vector2(); } \
static void DestroyAt(void* ptr) { static_cast<core::Vector2*>(ptr)->~Vector2(); } \

#undef GENERATED_BODY_IMPL_Rotator
#define GENERATED_BODY_IMPL_Rotator \
static void ConstructAt(void* ptr) { new (ptr) core::Rotator(); } \
static void DestroyAt(void* ptr) { static_cast<core::Rotator*>(ptr)->~Rotator(); } \

#undef GENERATED_BODY_IMPL_Color
#define GENERATED_BODY_IMPL_Color \
static void ConstructAt(void* ptr) { new (ptr) core::Color(); } \
static void DestroyAt(void* ptr) { static_cast<core::Color*>(ptr)->~Color(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::Size2D::REFLECTION_Register_Size2D_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Size2D>("core.Size2D")->SetAttribute(Type::Trivial); \
type->Internal_RegisterField("width", &core::Size2D::width, offsetof(core::Size2D, width)); \
type->Internal_RegisterField("height", &core::Size2D::height, offsetof(core::Size2D, height)); \
return type; \
} \
core::Type* core::Vector2::REFLECTION_Register_Vector2_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Vector2>("core.Vector2")->SetAttribute(Type::Trivial); \
type->Internal_RegisterField("x", &core::Vector2::x, offsetof(core::Vector2, x)); \
type->Internal_RegisterField("y", &core::Vector2::y, offsetof(core::Vector2, y)); \
return type; \
} \
core::Type* core::Rotator::REFLECTION_Register_Rotator_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Rotator>("core.Rotator")->SetAttribute(Type::Trivial); \
type->Internal_RegisterField("yaw", &core::Rotator::yaw, offsetof(core::Rotator, yaw)); \
type->Internal_RegisterField("roll", &core::Rotator::roll, offsetof(core::Rotator, roll)); \
type->Internal_RegisterField("pitch", &core::Rotator::pitch, offsetof(core::Rotator, pitch)); \
return type; \
} \
core::Type* core::Color::REFLECTION_Register_Color_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Color>("core.Color")->SetAttribute(Type::Trivial); \
type->Internal_RegisterField("r", &core::Color::r, offsetof(core::Color, r)); \
type->Internal_RegisterField("g", &core::Color::g, offsetof(core::Color, g)); \
type->Internal_RegisterField("b", &core::Color::b, offsetof(core::Color, b)); \
type->Internal_RegisterField("a", &core::Color::a, offsetof(core::Color, a)); \
return type; \
} \

