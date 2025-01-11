// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::Camera>(), &func::Camera::REFLECTION_Register_Camera_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::Camera>(), &func::Camera::ConstructAt, &func::Camera::DestroyAt); \

#undef GENERATED_BODY_IMPL_Camera
#define GENERATED_BODY_IMPL_Camera \
static void ConstructAt(void* ptr) { new (ptr) func::Camera(); } \
static void DestroyAt(void* ptr) { static_cast<func::Camera*>(ptr)->~Camera(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::Camera::REFLECTION_Register_Camera_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::Camera>("func.Camera")->Internal_AddParent(TypeOf<func::Actor>())->SetComment("Object不自动生成默认构造函数"); \
return type; \
} \

