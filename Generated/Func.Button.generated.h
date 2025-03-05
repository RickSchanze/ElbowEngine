// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::Button>(), &func::ui::widget::Button::REFLECTION_Register_Button_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::Button>(), &func::ui::widget::Button::ConstructAt, &func::ui::widget::Button::DestroyAt); \

#undef GENERATED_BODY_IMPL_Button
#define GENERATED_BODY_IMPL_Button \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::Button(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::Button*>(ptr)->~Button(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::widget::Button::REFLECTION_Register_Button_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::Button>("func.ui.widget.Button")->Internal_AddParent(TypeOf<func::ui::widget::Widget>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("text_", &func::ui::widget::Button::text_, offsetof(func::ui::widget::Button, text_)); \
type->Internal_RegisterField("padding_", &func::ui::widget::Button::padding_, offsetof(func::ui::widget::Button, padding_)); \
return type; \
} \

