//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Config/IConfig.hpp"


class FuncConfig : public IConfig {
    REFLECTED_CLASS(FuncConfig)

    DEFINE_CFG_ITEM(UInt64, preserved_ui_vertex_count, PreservedUIVertexCount, 10000);
    DEFINE_CFG_ITEM(UInt64, preserved_ui_index_count, PreservedUIIndexCount, 30000);
    DEFINE_CFG_ITEM(String, ui_style_path, UIStylePath, "Assets/Dark.yaml");
    // 这个数值使用在Instancing draw, 所有物体的Transform都存在这个buffer里通过instance draw绘制
    DEFINE_CFG_ITEM(UInt16, preallocated_instacned_data_count, PreallocatedInstacnedDataCount, 1000);
};

REGISTER_TYPE(FuncConfig)
