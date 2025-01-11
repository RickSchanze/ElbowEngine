// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::Transform>(), &func::Transform::REFLECTION_Register_Transform_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::Transform>(), &func::Transform::ConstructAt, &func::Transform::DestroyAt); \

#undef GENERATED_BODY_IMPL_Transform
#define GENERATED_BODY_IMPL_Transform \
static void ConstructAt(void* ptr) { new (ptr) func::Transform(); } \
static void DestroyAt(void* ptr) { static_cast<func::Transform*>(ptr)->~Transform(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::Transform::REFLECTION_Register_Transform_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::Transform>("func.Transform"); \
type->Internal_RegisterField("location", &func::Transform::location, offsetof(func::Transform, location))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "位置"); \
type->Internal_RegisterField("scale", &func::Transform::scale, offsetof(func::Transform, scale))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "缩放"); \
type->Internal_RegisterField("rotation", &func::Transform::rotation, offsetof(func::Transform, rotation))->SetComment("TODO: UI上的Rotation不应该用四元数")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "旋转"); \
return type; \
} \

