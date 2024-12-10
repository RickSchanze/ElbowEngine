// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<TestArchiveSerialization>(), &TestArchiveSerialization::REFLECTION_Register_TestArchiveSerialization_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<TestArchiveSerialization>(), &TestArchiveSerialization::ConstructAt, &TestArchiveSerialization::DestroyAt); \

#undef GENERATED_BODY_IMPL_TestArchiveSerialization
#define GENERATED_BODY_IMPL_TestArchiveSerialization \
static void ConstructAt(void* ptr) { new (ptr) TestArchiveSerialization(); } \
static void DestroyAt(void* ptr) { static_cast<TestArchiveSerialization*>(ptr)->~TestArchiveSerialization(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* TestArchiveSerialization::REFLECTION_Register_TestArchiveSerialization_Registerer() { \
using namespace core; \
Type* type = Type::Create<TestArchiveSerialization>("TestArchiveSerialization")->Internal_AddParent(TypeOf<core::ITypeGetter>()); \
type->Internal_RegisterField("a_", &TestArchiveSerialization::a_, offsetof(TestArchiveSerialization, a_)); \
type->Internal_RegisterField("b_", &TestArchiveSerialization::b_, offsetof(TestArchiveSerialization, b_)); \
type->Internal_RegisterField("c_", &TestArchiveSerialization::c_, offsetof(TestArchiveSerialization, c_)); \
type->Internal_RegisterField("d_", &TestArchiveSerialization::d_, offsetof(TestArchiveSerialization, d_)); \
type->Internal_RegisterField("e_", &TestArchiveSerialization::e_, offsetof(TestArchiveSerialization, e_)); \
type->Internal_RegisterField("f_", &TestArchiveSerialization::f_, offsetof(TestArchiveSerialization, f_)); \
type->Internal_RegisterField("g_", &TestArchiveSerialization::g_, offsetof(TestArchiveSerialization, g_)); \
return type; \
} \

