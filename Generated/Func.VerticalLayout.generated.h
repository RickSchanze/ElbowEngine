// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::VerticalLayout>(), &func::ui::widget::VerticalLayout::REFLECTION_Register_VerticalLayout_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::VerticalLayout>(), &func::ui::widget::VerticalLayout::ConstructAt, &func::ui::widget::VerticalLayout::DestroyAt); \

#undef GENERATED_BODY_IMPL_VerticalLayout
#define GENERATED_BODY_IMPL_VerticalLayout \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::VerticalLayout(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::VerticalLayout*>(ptr)->~VerticalLayout(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::widget::VerticalLayout::REFLECTION_Register_VerticalLayout_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::VerticalLayout>("func.ui.widget.VerticalLayout")->Internal_AddParent(TypeOf<func::ui::widget::Widget>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("children_", &func::ui::widget::VerticalLayout::children_, offsetof(func::ui::widget::VerticalLayout, children_)); \
type->Internal_RegisterField("vertical_alignment_", &func::ui::widget::VerticalLayout::vertical_alignment_, offsetof(func::ui::widget::VerticalLayout, vertical_alignment_)); \
return type; \
} \

