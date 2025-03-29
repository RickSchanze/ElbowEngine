//
// Created by Echo on 2025/3/29.
//

#include "FuncConfig.hpp"

IMPL_REFLECTED(FuncConfig) {
    return Type::Create<FuncConfig>("FuncConfig") | refl_helper::Attribute(Type::ValueAttribute::Config, "Config/Func.cfg") |
           refl_helper::AddParents<IConfig>() | refl_helper::AddField("preserved_ui_index_count", &ThisClass::preserved_ui_vertex_count) |
           refl_helper::AddField("preserved_ui_index_count", &ThisClass::preserved_ui_index_count) |
           refl_helper::AddField("ui_style_path", &ThisClass::ui_style_path);
}
