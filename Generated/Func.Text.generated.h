// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::ui::widget::Text>(), &func::ui::widget::Text::REFLECTION_Register_Text_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::ui::widget::Text>(), &func::ui::widget::Text::ConstructAt, &func::ui::widget::Text::DestroyAt); \

#undef GENERATED_BODY_IMPL_Text
#define GENERATED_BODY_IMPL_Text \
static void ConstructAt(void* ptr) { new (ptr) func::ui::widget::Text(); } \
static void DestroyAt(void* ptr) { static_cast<func::ui::widget::Text*>(ptr)->~Text(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::ui::widget::Text::REFLECTION_Register_Text_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::ui::widget::Text>("func.ui.widget.Text")->Internal_AddParent(TypeOf<func::ui::widget::Widget>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("text_", &func::ui::widget::Text::text_, offsetof(func::ui::widget::Text, text_))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "文本"); \
type->Internal_RegisterField("spacing_", &func::ui::widget::Text::spacing_, offsetof(func::ui::widget::Text, spacing_))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "字符间距"); \
type->Internal_RegisterField("size_", &func::ui::widget::Text::size_, offsetof(func::ui::widget::Text, size_))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "大小"); \
type->Internal_RegisterField("base_line_", &func::ui::widget::Text::base_line_, offsetof(func::ui::widget::Text, base_line_))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "基线位置"); \
type->Internal_RegisterField("font_", &func::ui::widget::Text::font_, offsetof(func::ui::widget::Text, font_))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "字体"); \
type->Internal_RegisterField("font_material_", &func::ui::widget::Text::font_material_, offsetof(func::ui::widget::Text, font_material_))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "字体材质"); \
type->Internal_RegisterField("size_base_", &func::ui::widget::Text::size_base_, offsetof(func::ui::widget::Text, size_base_))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "大小基准")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Advance"); \
return type; \
} \

