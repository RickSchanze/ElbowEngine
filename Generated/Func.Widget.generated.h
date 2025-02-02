// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::Widget>(), &func::ui::widget::Widget::REFLECTION_Register_Widget_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::Widget>(), &func::ui::widget::Widget::ConstructAt, &func::ui::widget::Widget::DestroyAt); \

#undef GENERATED_BODY_IMPL_Widget
#define GENERATED_BODY_IMPL_Widget \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::Widget(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::Widget*>(ptr)->~Widget(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::widget::Widget::REFLECTION_Register_Widget_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::Widget>("func.ui.widget.Widget")->Internal_AddParent(TypeOf<core::Object>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

