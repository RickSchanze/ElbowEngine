// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::WindowPanel>(), &func::ui::widget::WindowPanel::REFLECTION_Register_WindowPanel_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::WindowPanel>(), &func::ui::widget::WindowPanel::ConstructAt, &func::ui::widget::WindowPanel::DestroyAt); \

#undef GENERATED_BODY_IMPL_WindowPanel
#define GENERATED_BODY_IMPL_WindowPanel \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::WindowPanel(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::WindowPanel*>(ptr)->~WindowPanel(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::widget::WindowPanel::REFLECTION_Register_WindowPanel_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::WindowPanel>("func.ui.widget.WindowPanel")->Internal_AddParent(TypeOf<func::ui::widget::Panel>())->SetComment("Panel是一个覆盖整个TargetRect的Widget"); \
type->Internal_RegisterField("title_", &func::ui::widget::WindowPanel::title_, offsetof(func::ui::widget::WindowPanel, title_)); \
type->Internal_RegisterField("title_height_", &func::ui::widget::WindowPanel::title_height_, offsetof(func::ui::widget::WindowPanel, title_height_)); \
type->Internal_RegisterField("slot_", &func::ui::widget::WindowPanel::slot_, offsetof(func::ui::widget::WindowPanel, slot_)); \
return type; \
} \

