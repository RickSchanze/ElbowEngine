﻿// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<platform::PlatformConfig>(), &platform::PlatformConfig::REFLECTION_Register_PlatformConfig_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<platform::PlatformConfig>(), &platform::PlatformConfig::ConstructAt, &platform::PlatformConfig::DestroyAt); \

#undef GENERATED_BODY_IMPL_PlatformConfig
#define GENERATED_BODY_IMPL_PlatformConfig \
static void ConstructAt(void* ptr) { new (ptr) platform::PlatformConfig(); } \
static void DestroyAt(void* ptr) { static_cast<platform::PlatformConfig*>(ptr)->~PlatformConfig(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* platform::PlatformConfig::REFLECTION_Register_PlatformConfig_Registerer() { \
using namespace core; \
Type* type = Type::Create<platform::PlatformConfig>("platform.PlatformConfig")->Internal_AddParent(TypeOf<core::IConfig>())->SetAttribute(core::Type::ValueAttribute::Config, "Config/Platform/PlatformConfig.cfg")->SetAttribute(core::Type::ValueAttribute::Category, "Platform"); \
type->Internal_RegisterField("api", &platform::PlatformConfig::api, offsetof(platform::PlatformConfig, api))->SetComment("使用哪个API")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "使用图形API")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform|RHI"); \
type->Internal_RegisterField("window_lib", &platform::PlatformConfig::window_lib, offsetof(platform::PlatformConfig, window_lib))->SetComment("窗口库")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "使用的窗口库")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform"); \
type->Internal_RegisterField("default_window_size", &platform::PlatformConfig::default_window_size, offsetof(platform::PlatformConfig, default_window_size))->SetComment("窗口库")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "默认窗口大小")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform"); \
type->Internal_RegisterField("window_flag", &platform::PlatformConfig::window_flag, offsetof(platform::PlatformConfig, window_flag))->SetComment("窗口库")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "默认窗口属性")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform")->SetAttribute(core::FieldInfo::ValueAttribute::EnumFlag, "platform.WindowFlag"); \
type->Internal_RegisterField("msaa_sample_count", &platform::PlatformConfig::msaa_sample_count, offsetof(platform::PlatformConfig, msaa_sample_count))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "MSAA采样次数")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform|RHI")->SetAttribute(core::FieldInfo::ValueAttribute::EnumFlag, "platform.rhi.SampleCount"); \
type->Internal_RegisterField("present_mode", &platform::PlatformConfig::present_mode, offsetof(platform::PlatformConfig, present_mode))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "呈现方式")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform|RHI"); \
type->Internal_RegisterField("swapchain_image_count", &platform::PlatformConfig::swapchain_image_count, offsetof(platform::PlatformConfig, swapchain_image_count))->SetComment("交换链图像数量")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "交换链图像数量")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform|RHI"); \
type->Internal_RegisterField("enable_validation_layer", &platform::PlatformConfig::enable_validation_layer, offsetof(platform::PlatformConfig, enable_validation_layer))->SetComment("是否开启验证层")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "开启验证层")->SetAttribute(core::FieldInfo::ValueAttribute::EnableWhen, "api == GraphicsAPI::Vulkan")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform|RHI|Vulkan"); \
type->Internal_RegisterField("validation_layer_name", &platform::PlatformConfig::validation_layer_name, offsetof(platform::PlatformConfig, validation_layer_name))->SetComment("验证层名字 一般不用修改")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "验证层名字")->SetAttribute(core::FieldInfo::ValueAttribute::EnableWhen, "api == GraphicsAPI::Vulkan")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform|RHI|Vulkan"); \
type->Internal_RegisterField("required_device_extensions", &platform::PlatformConfig::required_device_extensions, offsetof(platform::PlatformConfig, required_device_extensions))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "需要的扩展")->SetAttribute(core::FieldInfo::ValueAttribute::EnableWhen, "api == GraphicsAPI::Vulkan")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Platform|RHI|Vulkan"); \
return type; \
} \
