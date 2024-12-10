// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
{ \
Type* type = Type::Create<platform::rhi::Format>("platform.rhi.Format"); \
using enum platform::rhi::Format; \
type->Internal_RegisterEnumValue(R32G32B32_Float ,"R32G32B32_Float")->SetComment("每个元素由3个32位浮点数分量构成"); \
type->Internal_RegisterEnumValue(R16G16B16A16_UNorm ,"R16G16B16A16_UNorm")->SetComment("每个元素由4个16位分量构成, 每个分量被映射到[0, 1]"); \
type->Internal_RegisterEnumValue(R32G32_UInt ,"R32G32_UInt")->SetComment("每个元素由2个32位无符号整数构成"); \
type->Internal_RegisterEnumValue(R8G8B8A8_UNorm ,"R8G8B8A8_UNorm")->SetComment("每个元素由4个8位无符号数构成, 被映射到[0, 1]"); \
type->Internal_RegisterEnumValue(R8G8B8A8_SNorm ,"R8G8B8A8_SNorm")->SetComment("每个元素由4个8位有符号数构成, 被映射到[-1, 1]"); \
type->Internal_RegisterEnumValue(R8G8B8A8_UInt ,"R8G8B8A8_UInt")->SetComment("每个元素由4个8位无符号数构成, 被映射到[0, 255]"); \
type->Internal_RegisterEnumValue(R8G8B8A8_SInt ,"R8G8B8A8_SInt")->SetComment("每个元素由4个8位有符号数构成, 被映射到[-128, 127]"); \
type->Internal_RegisterEnumValue(D32_Float_S8X24_UInt ,"D32_Float_S8X24_UInt")->SetComment("深度/模版: 64位 32位给深度, 8位给模版([0, 255]), 24位用作对齐"); \
type->Internal_RegisterEnumValue(D32_Float ,"D32_Float")->SetComment("深度: 32位浮点数"); \
type->Internal_RegisterEnumValue(D24_UNorm_S8_UInt ,"D24_UNorm_S8_UInt")->SetComment("深度/模版: 24位给深度([0, 1]), 8位给模版([0, 255])"); \
type->Internal_RegisterEnumValue(D16_UNorm ,"D16_UNorm")->SetComment("深度: 16位([0, 1])"); \
type->Internal_RegisterEnumValue(B8G8R8A8_SRGB ,"B8G8R8A8_SRGB")->SetComment("每个元素由4个8位分量构成, 被映射到sRGB"); \
type->Internal_RegisterEnumValue(B8G8R8A8_UNorm ,"B8G8R8A8_UNorm")->SetComment("每个元素由4个8位分量构成, 被映射到[0, 1]"); \
type->Internal_RegisterEnumValue(R16G16B16A16_Float ,"R16G16B16A16_Float")->SetComment("每个元素由4个16位float分量构成"); \
type->Internal_RegisterEnumValue(A2B10G10R10_UNorm ,"A2B10G10R10_UNorm")->SetComment("a: 2位 b: 10位 g: 10位 r: 10位"); \
type->Internal_RegisterEnumValue(Count ,"Count")->SetComment("超出范围(Undefined)"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<platform::rhi::Format>(), type); \
} \
{ \
Type* type = Type::Create<platform::rhi::ColorSpace>("platform.rhi.ColorSpace"); \
using enum platform::rhi::ColorSpace; \
type->Internal_RegisterEnumValue(sRGB ,"sRGB")->SetComment("非线性sRGB, 适用于大部分普通内容, 标准显示器"); \
type->Internal_RegisterEnumValue(HDR10 ,"HDR10")->SetComment("HDR10"); \
type->Internal_RegisterEnumValue(Count ,"Count")->SetComment("超出范围"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<platform::rhi::ColorSpace>(), type); \
} \
{ \
Type* type = Type::Create<platform::rhi::PresentMode>("platform.rhi.PresentMode"); \
using enum platform::rhi::PresentMode; \
type->Internal_RegisterEnumValue(VSync ,"VSync")->SetComment("垂直同步"); \
type->Internal_RegisterEnumValue(Immediate ,"Immediate")->SetComment("立即刷新"); \
type->Internal_RegisterEnumValue(TripleBuffer ,"TripleBuffer")->SetComment("三重缓冲"); \
type->Internal_RegisterEnumValue(Count ,"Count")->SetComment("超出范围"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<platform::rhi::PresentMode>(), type); \
} \
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
{ \
Type* type = Type::Create<platform::rhi::SampleCount>("platform.rhi.SampleCount")->SetComment("MSAA采样次数\nVulkan API指示这里是个Flag Enum, Follow it.")->SetAttribute(Type::FlagAttribute::Flag); \
using enum platform::rhi::SampleCount; \
type->Internal_RegisterEnumValue(SC_1 ,"SC_1")->SetComment("1次"); \
type->Internal_RegisterEnumValue(SC_2 ,"SC_2")->SetComment("2次"); \
type->Internal_RegisterEnumValue(SC_4 ,"SC_4")->SetComment("4次"); \
type->Internal_RegisterEnumValue(SC_8 ,"SC_8")->SetComment("8次"); \
type->Internal_RegisterEnumValue(SC_16 ,"SC_16")->SetComment("16次"); \
type->Internal_RegisterEnumValue(SC_32 ,"SC_32")->SetComment("32次"); \
type->Internal_RegisterEnumValue(SC_64 ,"SC_64")->SetComment("64次"); \
type->Internal_RegisterEnumValue(SC_Count ,"SC_Count")->SetComment("超出范围"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<platform::rhi::SampleCount>(), type); \
} \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \

