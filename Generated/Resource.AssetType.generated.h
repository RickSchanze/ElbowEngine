// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
{ \
Type* type = Type::Create<resource::AssetType>("resource.AssetType"); \
using enum resource::AssetType; \
type->Internal_RegisterEnumValue(Mesh ,"Mesh")->SetAttribute(FieldInfo::ValueAttribute::Label, "网格"); \
type->Internal_RegisterEnumValue(Shader ,"Shader")->SetAttribute(FieldInfo::ValueAttribute::Label, "着色器"); \
type->Internal_RegisterEnumValue(Texture ,"Texture")->SetAttribute(FieldInfo::ValueAttribute::Label, "纹理"); \
type->Internal_RegisterEnumValue(Material ,"Material")->SetAttribute(FieldInfo::ValueAttribute::Label, "材质"); \
type->Internal_RegisterEnumValue(Animation ,"Animation")->SetAttribute(FieldInfo::ValueAttribute::Label, "动画"); \
type->Internal_RegisterEnumValue(Audio ,"Audio")->SetAttribute(FieldInfo::ValueAttribute::Label, "音频"); \
type->Internal_RegisterEnumValue(Font ,"Font")->SetAttribute(FieldInfo::ValueAttribute::Label, "字体"); \
type->Internal_RegisterEnumValue(Scene ,"Scene")->SetAttribute(FieldInfo::ValueAttribute::Label, "场景"); \
type->Internal_RegisterEnumValue(Prefab ,"Prefab")->SetAttribute(FieldInfo::ValueAttribute::Label, "预制体"); \
type->Internal_RegisterEnumValue(Count ,"Count")->SetAttribute(FieldInfo::ValueAttribute::Label, "超出范围"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<resource::AssetType>(), type); \
} \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \

