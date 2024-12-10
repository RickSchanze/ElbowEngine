// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::IConfig>(), &core::IConfig::REFLECTION_Register_IConfig_Registerer); \

#undef GENERATED_BODY_IMPL_IConfig
#define GENERATED_BODY_IMPL_IConfig \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::IConfig::REFLECTION_Register_IConfig_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::IConfig>("core.IConfig")->Internal_AddParent(TypeOf<core::Interface>())->SetAttribute(Type::Interface); \
return type; \
} \

