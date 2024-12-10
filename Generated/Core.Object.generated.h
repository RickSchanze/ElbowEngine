// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Object>(), &core::Object::REFLECTION_Register_Object_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Object>(), &core::Object::ConstructAt, &core::Object::DestroyAt); \

#undef GENERATED_BODY_IMPL_Object
#define GENERATED_BODY_IMPL_Object \
static void ConstructAt(void* ptr) { new (ptr) core::Object(); } \
static void DestroyAt(void* ptr) { static_cast<core::Object*>(ptr)->~Object(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::Object::REFLECTION_Register_Object_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Object>("core.Object"); \
type->Internal_RegisterField("name_", &core::Object::name_, offsetof(core::Object, name_)); \
type->Internal_RegisterField("display_name_", &core::Object::display_name_, offsetof(core::Object, display_name_)); \
return type; \
} \

