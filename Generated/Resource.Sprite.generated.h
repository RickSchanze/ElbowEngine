// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Sprite>(), &resource::Sprite::REFLECTION_Register_Sprite_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Sprite>(), &resource::Sprite::ConstructAt, &resource::Sprite::DestroyAt); \

#undef GENERATED_BODY_IMPL_Sprite
#define GENERATED_BODY_IMPL_Sprite \
static void ConstructAt(void* ptr) { new (ptr) resource::Sprite(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Sprite*>(ptr)->~Sprite(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Sprite::REFLECTION_Register_Sprite_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Sprite>("resource.Sprite")->SetAttribute(Type::Trivial); \
type->Internal_RegisterField("id_", &resource::Sprite::id_, offsetof(resource::Sprite, id_)); \
type->Internal_RegisterField("target_texture_", &resource::Sprite::target_texture_, offsetof(resource::Sprite, target_texture_)); \
return type; \
} \

