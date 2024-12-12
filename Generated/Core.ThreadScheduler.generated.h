// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
{ \
Type* type = Type::Create<core::ThreadSlot>("core.ThreadSlot"); \
using enum core::ThreadSlot; \
type->Internal_RegisterEnumValue(Game ,"Game")->SetComment("Game线程(主线程)非常特殊, 不在ThreadSlot使用, 使用GameLoopScheduler"); \
type->Internal_RegisterEnumValue(Render ,"Render"); \
type->Internal_RegisterEnumValue(Resource ,"Resource"); \
type->Internal_RegisterEnumValue(Other ,"Other"); \
type->Internal_RegisterEnumValue(Count ,"Count"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<core::ThreadSlot>(), type); \
} \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \

