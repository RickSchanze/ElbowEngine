// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Project>(), &resource::Project::REFLECTION_Register_Project_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Project>(), &resource::Project::ConstructAt, &resource::Project::DestroyAt); \

#undef GENERATED_BODY_IMPL_Project
#define GENERATED_BODY_IMPL_Project \
static void ConstructAt(void* ptr) { new (ptr) resource::Project(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Project*>(ptr)->~Project(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Project::REFLECTION_Register_Project_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Project>("resource.Project")->SetComment("Project阐述了一个项目的基本信息\n例如 项目名称 项目路径 项目版本号 项目数据库位置等"); \
type->Internal_RegisterField("name_", &resource::Project::name_, offsetof(resource::Project, name_)); \
type->Internal_RegisterField("path_", &resource::Project::path_, offsetof(resource::Project, path_)); \
type->Internal_RegisterField("version_", &resource::Project::version_, offsetof(resource::Project, version_)); \
type->Internal_RegisterField("database_path_", &resource::Project::database_path_, offsetof(resource::Project, database_path_))->SetComment("这个是资产数据库的路径"); \
return type; \
} \

