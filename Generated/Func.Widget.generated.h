// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::Padding>(), &func::ui::widget::Padding::REFLECTION_Register_Padding_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::Padding>(), &func::ui::widget::Padding::ConstructAt, &func::ui::widget::Padding::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::Widget>(), &func::ui::widget::Widget::REFLECTION_Register_Widget_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::Widget>(), &func::ui::widget::Widget::ConstructAt, &func::ui::widget::Widget::DestroyAt); \
{ \
Type* type = Type::Create<func::ui::widget::HorizontalAlignment>("func.ui.widget.HorizontalAlignment"); \
using enum func::ui::widget::HorizontalAlignment; \
type->Internal_RegisterEnumValue(Left ,"Left"); \
type->Internal_RegisterEnumValue(Center ,"Center"); \
type->Internal_RegisterEnumValue(Right ,"Right"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<func::ui::widget::HorizontalAlignment>(), type); \
} \
{ \
Type* type = Type::Create<func::ui::widget::VerticalAlignment>("func.ui.widget.VerticalAlignment"); \
using enum func::ui::widget::VerticalAlignment; \
type->Internal_RegisterEnumValue(Top ,"Top"); \
type->Internal_RegisterEnumValue(Center ,"Center"); \
type->Internal_RegisterEnumValue(Bottom ,"Bottom"); \
core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<func::ui::widget::VerticalAlignment>(), type); \
} \

#undef GENERATED_BODY_IMPL_Padding
#define GENERATED_BODY_IMPL_Padding \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::Padding(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::Padding*>(ptr)->~Padding(); } \

#undef GENERATED_BODY_IMPL_Widget
#define GENERATED_BODY_IMPL_Widget \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::Widget(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::Widget*>(ptr)->~Widget(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::widget::Padding::REFLECTION_Register_Padding_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::Padding>("func.ui.widget.Padding"); \
type->Internal_RegisterField("left", &func::ui::widget::Padding::left, offsetof(func::ui::widget::Padding, left)); \
type->Internal_RegisterField("top", &func::ui::widget::Padding::top, offsetof(func::ui::widget::Padding, top)); \
type->Internal_RegisterField("right", &func::ui::widget::Padding::right, offsetof(func::ui::widget::Padding, right)); \
type->Internal_RegisterField("bottom", &func::ui::widget::Padding::bottom, offsetof(func::ui::widget::Padding, bottom)); \
return type; \
} \
core::Type* func::ui::widget::Widget::REFLECTION_Register_Widget_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::Widget>("func.ui.widget.Widget")->Internal_AddParent(TypeOf<core::Object>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

