// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
{ \
Type* type = Type::Create<platform::rhi::GraphicsAPI>("platform.rhi.GraphicsAPI"); \
using enum platform::rhi::GraphicsAPI; \
type->Internal_RegisterEnumValue(Vulkan ,"Vulkan"); \
type->Internal_RegisterEnumValue(D3D12 ,"D3D12"); \
type->Internal_RegisterEnumValue(OpenGL ,"OpenGL"); \
type->Internal_RegisterEnumValue(Null ,"Null"); \
type->Internal_RegisterEnumValue(Count ,"Count"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<platform::rhi::GraphicsAPI>(), type); \
} \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \

