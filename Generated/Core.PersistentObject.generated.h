// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::PersistentObject>(), &core::PersistentObject::REFLECTION_Register_PersistentObject_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::PersistentObject>(), &core::PersistentObject::ConstructAt, &core::PersistentObject::DestroyAt); \

#undef GENERATED_BODY_IMPL_PersistentObject
#define GENERATED_BODY_IMPL_PersistentObject \
static void ConstructAt(void* ptr) { new (ptr) core::PersistentObject(); } \
static void DestroyAt(void* ptr) { static_cast<core::PersistentObject*>(ptr)->~PersistentObject(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::PersistentObject::REFLECTION_Register_PersistentObject_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::PersistentObject>("core.PersistentObject")->Internal_AddParent(TypeOf<core::Object>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

