// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
{ \
Type* type = Type::Create<platform::WindowLib>("platform.WindowLib"); \
using enum platform::WindowLib; \
type->Internal_RegisterEnumValue(GLFW ,"GLFW"); \
type->Internal_RegisterEnumValue(SDL3 ,"SDL3"); \
type->Internal_RegisterEnumValue(Count ,"Count"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<platform::WindowLib>(), type); \
} \
{ \
Type* type = Type::Create<platform::WindowFlag>("platform.WindowFlag")->SetAttribute(Type::FlagAttribute::Flag); \
using enum platform::WindowFlag; \
type->Internal_RegisterEnumValue(WF_NoWindowTitle ,"WF_NoWindowTitle")->SetComment("窗口没有标题栏"); \
type->Internal_RegisterEnumValue(WF_NoResize ,"WF_NoResize")->SetComment("窗口不能调整大小"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<platform::WindowFlag>(), type); \
} \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \

