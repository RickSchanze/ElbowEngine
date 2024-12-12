// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<TestMetaInfoGenerate>(), &TestMetaInfoGenerate::REFLECTION_Register_TestMetaInfoGenerate_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<TestMetaInfoGenerate>(), &TestMetaInfoGenerate::ConstructAt, &TestMetaInfoGenerate::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::TestMetaInfoGenerate1>(), &core::TestMetaInfoGenerate1::REFLECTION_Register_TestMetaInfoGenerate1_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::TestMetaInfoGenerate1>(), &core::TestMetaInfoGenerate1::ConstructAt, &core::TestMetaInfoGenerate1::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::inner::TestMetaInfoGenerate2>(), &core::inner::TestMetaInfoGenerate2::REFLECTION_Register_TestMetaInfoGenerate2_Registerer); \
{ \
Type* type = Type::Create<core::TestEnum>("core.TestEnum"); \
using enum core::TestEnum; \
type->Internal_RegisterEnumValue(A ,"A"); \
type->Internal_RegisterEnumValue(B ,"B"); \
type->Internal_RegisterEnumValue(C ,"C"); \
type->Internal_RegisterEnumValue(D ,"D"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<core::TestEnum>(), type); \
} \

#undef GENERATED_BODY_IMPL_TestMetaInfoGenerate
#define GENERATED_BODY_IMPL_TestMetaInfoGenerate \
static void ConstructAt(void* ptr) { new (ptr) TestMetaInfoGenerate(); } \
static void DestroyAt(void* ptr) { static_cast<TestMetaInfoGenerate*>(ptr)->~TestMetaInfoGenerate(); } \

#undef GENERATED_BODY_IMPL_TestMetaInfoGenerate1
#define GENERATED_BODY_IMPL_TestMetaInfoGenerate1 \
static void ConstructAt(void* ptr) { new (ptr) core::TestMetaInfoGenerate1(); } \
static void DestroyAt(void* ptr) { static_cast<core::TestMetaInfoGenerate1*>(ptr)->~TestMetaInfoGenerate1(); } \

#undef GENERATED_BODY_IMPL_TestMetaInfoGenerate2
#define GENERATED_BODY_IMPL_TestMetaInfoGenerate2 \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* TestMetaInfoGenerate::REFLECTION_Register_TestMetaInfoGenerate_Registerer() { \
using namespace core; \
Type* type = Type::Create<TestMetaInfoGenerate>("TestMetaInfoGenerate")->SetComment("测试TestMetaInfoGenerate"); \
type->Internal_RegisterField("value1_", &TestMetaInfoGenerate::value1_, offsetof(TestMetaInfoGenerate, value1_))->SetComment("测试属性注释"); \
type->Internal_RegisterField("value2_", &TestMetaInfoGenerate::value2_, offsetof(TestMetaInfoGenerate, value2_)); \
type->Internal_RegisterField("value3_", &TestMetaInfoGenerate::value3_, offsetof(TestMetaInfoGenerate, value3_)); \
type->Internal_RegisterField("value4_", &TestMetaInfoGenerate::value4_, offsetof(TestMetaInfoGenerate, value4_)); \
return type; \
} \
core::Type* core::TestMetaInfoGenerate1::REFLECTION_Register_TestMetaInfoGenerate1_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::TestMetaInfoGenerate1>("core.TestMetaInfoGenerate1"); \
type->Internal_RegisterField("value1_", &core::TestMetaInfoGenerate1::value1_, offsetof(core::TestMetaInfoGenerate1, value1_)); \
type->Internal_RegisterField("value2_", &core::TestMetaInfoGenerate1::value2_, offsetof(core::TestMetaInfoGenerate1, value2_)); \
type->Internal_RegisterField("value3_", &core::TestMetaInfoGenerate1::value3_, offsetof(core::TestMetaInfoGenerate1, value3_)); \
type->Internal_RegisterField("value4_", &core::TestMetaInfoGenerate1::value4_, offsetof(core::TestMetaInfoGenerate1, value4_)); \
return type; \
} \
core::Type* core::inner::TestMetaInfoGenerate2::REFLECTION_Register_TestMetaInfoGenerate2_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::inner::TestMetaInfoGenerate2>("NewName")->SetAttribute(Type::Interface); \
type->Internal_RegisterField("value1_", &core::inner::TestMetaInfoGenerate2::value1_, offsetof(core::inner::TestMetaInfoGenerate2, value1_)); \
type->Internal_RegisterField("value2_", &core::inner::TestMetaInfoGenerate2::value2_, offsetof(core::inner::TestMetaInfoGenerate2, value2_)); \
type->Internal_RegisterField("value3_", &core::inner::TestMetaInfoGenerate2::value3_, offsetof(core::inner::TestMetaInfoGenerate2, value3_)); \
type->Internal_RegisterField("value4_", &core::inner::TestMetaInfoGenerate2::value4_, offsetof(core::inner::TestMetaInfoGenerate2, value4_)); \
return type; \
} \

