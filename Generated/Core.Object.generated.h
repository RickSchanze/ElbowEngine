// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Object>(), &core::Object::REFLECTION_Register_Object_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Object>(), &core::Object::ConstructAt, &core::Object::DestroyAt); \
{ \
Type* type = Type::Create<core::ObjectFlagType>("core.ObjectFlagType")->SetAttribute(Type::FlagAttribute::Flag); \
using enum core::ObjectFlagType; \
type->Internal_RegisterEnumValue(OFT_Persistent ,"OFT_Persistent")->SetComment("此对象需要持久化存储"); \
type->Internal_RegisterEnumValue(OFT_Actor ,"OFT_Actor"); \
type->Internal_RegisterEnumValue(OFT_Component ,"OFT_Component"); \
type->Internal_RegisterEnumValue(OFT_Widget ,"OFT_Widget"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<core::ObjectFlagType>(), type); \
} \
{ \
Type* type = Type::Create<core::ObjectStateFlagType>("core.ObjectStateFlagType")->SetAttribute(Type::FlagAttribute::Flag); \
using enum core::ObjectStateFlagType; \
type->Internal_RegisterEnumValue(PendingKill ,"PendingKill"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<core::ObjectStateFlagType>(), type); \
} \

#undef GENERATED_BODY_IMPL_Object
#define GENERATED_BODY_IMPL_Object \
static void ConstructAt(void* ptr) { new (ptr) core::Object(); } \
static void DestroyAt(void* ptr) { static_cast<core::Object*>(ptr)->~Object(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::Object::REFLECTION_Register_Object_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Object>("core.Object")->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("handle_", &core::Object::handle_, offsetof(core::Object, handle_)); \
type->Internal_RegisterField("flags_", &core::Object::flags_, offsetof(core::Object, flags_)); \
type->Internal_RegisterField("state_", &core::Object::state_, offsetof(core::Object, state_)); \
type->Internal_RegisterField("name_", &core::Object::name_, offsetof(core::Object, name_)); \
type->Internal_RegisterField("display_name_", &core::Object::display_name_, offsetof(core::Object, display_name_)); \
return type; \
} \

