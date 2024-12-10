// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<platform::rhi::RHIConfig>(), &platform::rhi::RHIConfig::REFLECTION_Register_RHIConfig_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<platform::rhi::RHIConfig>(), &platform::rhi::RHIConfig::ConstructAt, &platform::rhi::RHIConfig::DestroyAt); \

#undef GENERATED_BODY_IMPL_RHIConfig
#define GENERATED_BODY_IMPL_RHIConfig \
static void ConstructAt(void* ptr) { new (ptr) platform::rhi::RHIConfig(); } \
static void DestroyAt(void* ptr) { static_cast<platform::rhi::RHIConfig*>(ptr)->~RHIConfig(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* platform::rhi::RHIConfig::REFLECTION_Register_RHIConfig_Registerer() { \
using namespace core; \
Type* type = Type::Create<platform::rhi::RHIConfig>("platform.rhi.RHIConfig")->Internal_AddParent(TypeOf<core::IConfig>())->SetAttribute(core::Type::ValueAttribute::Config, "Config/Platform/RHIConfig.cfg")->SetAttribute(core::Type::ValueAttribute::Category, "Platform"); \
type->Internal_RegisterField("api", &platform::rhi::RHIConfig::api, offsetof(platform::rhi::RHIConfig, api))->SetComment("使用哪个API")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "使用图形API"); \
type->Internal_RegisterField("swapchain_image_count", &platform::rhi::RHIConfig::swapchain_image_count, offsetof(platform::rhi::RHIConfig, swapchain_image_count))->SetComment("交换链图像数量")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "交换链图像数量"); \
type->Internal_RegisterField("enable_validation_layer", &platform::rhi::RHIConfig::enable_validation_layer, offsetof(platform::rhi::RHIConfig, enable_validation_layer))->SetComment("是否开启验证层")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "开启验证层")->SetAttribute(core::FieldInfo::ValueAttribute::EnableWhen, "api == GraphicsAPI::Vulkan")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Vulkan"); \
type->Internal_RegisterField("validation_layer_name", &platform::rhi::RHIConfig::validation_layer_name, offsetof(platform::rhi::RHIConfig, validation_layer_name))->SetComment("是否开启验证层")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "验证层名字")->SetAttribute(core::FieldInfo::ValueAttribute::EnableWhen, "api == GraphicsAPI::Vulkan")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Vulkan"); \
return type; \
} \

