// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Shader>(), &resource::Shader::REFLECTION_Register_Shader_Registerer); \
core::CtorManager::Get()->RegisterCtor(RTTITypeInfo::Create<resource::Shader>(), &resource::Shader::ConstructAt); \

#undef GENERATED_BODY_IMPL_Shader
#define GENERATED_BODY_IMPL_Shader \
static void ConstructAt(void* ptr) { new (ptr) resource::Shader(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Shader::REFLECTION_Register_Shader_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Shader>("resource.Shader")->Internal_AddParent(TypeOf<core::Object>())->Internal_AddParent(TypeOf<IResource>()); \
return type; \
} \

