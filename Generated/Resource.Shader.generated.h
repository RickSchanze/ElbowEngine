// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Shader>(), &resource::Shader::REFLECTION_Register_Shader_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Shader>(), &resource::Shader::ConstructAt, &resource::Shader::DestroyAt); \

#undef GENERATED_BODY_IMPL_Shader
#define GENERATED_BODY_IMPL_Shader \
static void ConstructAt(void* ptr) { new (ptr) resource::Shader(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Shader*>(ptr)->~Shader(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Shader::REFLECTION_Register_Shader_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Shader>("resource.Shader")->Internal_AddParent(TypeOf<resource::Asset>())->SetComment("对应一个slang文件\n一个slang可以通过[shader]语法声明多个shader\n不能有相同的stage"); \
return type; \
} \

