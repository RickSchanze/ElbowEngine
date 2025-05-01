//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Config/IConfig.hpp"

#include GEN_HEADER("FuncConfig.generated.hpp")

class ECLASS(Config = "Config/Func.cfg") FuncConfig : public IConfig
{
    GENERATED_BODY(FuncConfig)

    EFIELD()
    DEFINE_CFG_ITEM(UInt64, mPreservedUIVertexCount, PreservedUIVertexCount, 10000);

    EFIELD()
    DEFINE_CFG_ITEM(UInt64, mPreservedUIIndexCount, PreservedUIIndexCount, 30000);

    EFIELD()
    DEFINE_CFG_ITEM(String, mUIStylePath, UIStylePath, "Assets/Dark.yaml");

    // 这个数值使用在Instancing draw, 所有物体的Transform都存在这个buffer里通过instance draw绘制
    EFIELD()
    DEFINE_CFG_ITEM(UInt16, mPreallocatedInstacnedDataCount, PreallocatedInstacnedDataCount, 500);

    // 默认渲染管线的类型名称, 根据此名称查找渲染管线并设置
    EFIELD(Category = "Rendering")
    DEFINE_CFG_ITEM(String, mRenderPipelineName, RenderPipelineName, "")

};
