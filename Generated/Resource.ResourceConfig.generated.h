// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::ResourceConfig>(), &resource::ResourceConfig::REFLECTION_Register_ResourceConfig_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::ResourceConfig>(), &resource::ResourceConfig::ConstructAt, &resource::ResourceConfig::DestroyAt); \

#undef GENERATED_BODY_IMPL_ResourceConfig
#define GENERATED_BODY_IMPL_ResourceConfig \
static void ConstructAt(void* ptr) { new (ptr) resource::ResourceConfig(); } \
static void DestroyAt(void* ptr) { static_cast<resource::ResourceConfig*>(ptr)->~ResourceConfig(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::ResourceConfig::REFLECTION_Register_ResourceConfig_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::ResourceConfig>("resource.ResourceConfig")->Internal_AddParent(TypeOf<core::IConfig>())->SetAttribute(core::Type::ValueAttribute::Config, "Config/Resource.cfg"); \
type->Internal_RegisterField("shader_search_path", &resource::ResourceConfig::shader_search_path, offsetof(resource::ResourceConfig, shader_search_path))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "Shader搜索路径"); \
type->Internal_RegisterField("shader_intermediate_path", &resource::ResourceConfig::shader_intermediate_path, offsetof(resource::ResourceConfig, shader_intermediate_path))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "Shader中间产物生成路径"); \
return type; \
} \

