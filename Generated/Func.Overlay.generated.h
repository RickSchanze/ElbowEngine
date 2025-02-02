// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::Overlay>(), &func::ui::Overlay::REFLECTION_Register_Overlay_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::Overlay>(), &func::ui::Overlay::ConstructAt, &func::ui::Overlay::DestroyAt); \

#undef GENERATED_BODY_IMPL_Overlay
#define GENERATED_BODY_IMPL_Overlay \
static void ConstructAt(void* ptr) { new (ptr) func::ui::Overlay(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::Overlay*>(ptr)->~Overlay(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::Overlay::REFLECTION_Register_Overlay_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::Overlay>("func.ui.Overlay")->Internal_AddParent(TypeOf<core::Object>())->SetComment("理想情况下 一个Overlay一个DrawCall"); \
return type; \
} \

