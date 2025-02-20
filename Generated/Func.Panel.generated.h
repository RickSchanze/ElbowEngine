// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::Panel>(), &func::ui::widget::Panel::REFLECTION_Register_Panel_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::Panel>(), &func::ui::widget::Panel::ConstructAt, &func::ui::widget::Panel::DestroyAt); \

#undef GENERATED_BODY_IMPL_Panel
#define GENERATED_BODY_IMPL_Panel \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::Panel(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::Panel*>(ptr)->~Panel(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::widget::Panel::REFLECTION_Register_Panel_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::Panel>("func.ui.widget.Panel")->Internal_AddParent(TypeOf<func::ui::widget::Widget>())->SetComment("Panel是一个覆盖整个TargetRect的Widget"); \
type->Internal_RegisterField("sprite_", &func::ui::widget::Panel::sprite_, offsetof(func::ui::widget::Panel, sprite_))->SetComment("背景图像, 应是一个TextureAtlas里的一部分"); \
type->Internal_RegisterField("color_", &func::ui::widget::Panel::color_, offsetof(func::ui::widget::Panel, color_))->SetComment("附加的颜色"); \
type->Internal_RegisterField("material_", &func::ui::widget::Panel::material_, offsetof(func::ui::widget::Panel, material_))->SetComment("附加的材质"); \
type->Internal_RegisterField("position_", &func::ui::widget::Panel::position_, offsetof(func::ui::widget::Panel, position_)); \
type->Internal_RegisterField("size_", &func::ui::widget::Panel::size_, offsetof(func::ui::widget::Panel, size_)); \
return type; \
} \

