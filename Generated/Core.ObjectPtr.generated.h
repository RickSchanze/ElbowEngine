// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::ObjectPtrBase>(), &core::ObjectPtrBase::REFLECTION_Register_ObjectPtrBase_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::ObjectPtrBase>(), &core::ObjectPtrBase::ConstructAt, &core::ObjectPtrBase::DestroyAt); \

#undef GENERATED_BODY_IMPL_ObjectPtrBase
#define GENERATED_BODY_IMPL_ObjectPtrBase \
static void ConstructAt(void* ptr) { new (ptr) core::ObjectPtrBase(); } \
static void DestroyAt(void* ptr) { static_cast<core::ObjectPtrBase*>(ptr)->~ObjectPtrBase(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::ObjectPtrBase::REFLECTION_Register_ObjectPtrBase_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::ObjectPtrBase>("core.ObjectPtrBase"); \
type->Internal_RegisterField("object_", &core::ObjectPtrBase::object_, offsetof(core::ObjectPtrBase, object_))->SetComment("引用的Object"); \
return type; \
} \

