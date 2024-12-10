// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<IResource>(), &IResource::REFLECTION_Register_IResource_Registerer); \

#undef GENERATED_BODY_IMPL_IResource
#define GENERATED_BODY_IMPL_IResource \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* IResource::REFLECTION_Register_IResource_Registerer() { \
using namespace core; \
Type* type = Type::Create<IResource>("IResource")->Internal_AddParent(TypeOf<core::Interface>())->SetAttribute(Type::Interface); \
return type; \
} \

