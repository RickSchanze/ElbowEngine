//
// Created by Echo on 24-12-25.
//

#pragma once
#include "Core/Object/Object.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include <cstdint>

#include GEN_HEADER("Resource.ShaderMeta.generated.h")

namespace resource
{
class CLASS(SQLTable = "Shader") ShaderMeta
{
    GENERATED_CLASS(ShaderMeta)

public:
    PROPERTY(SQLAttr = "(PrimaryKey, AutoIncrement)")
    int32_t id;

    PROPERTY()
    core::ObjectHandle object_handle;

    PROPERTY()
    core::String path;

    // 下面可以写一些Shader可能需要的属性, 例如是否开启blend等
};
}   // namespace resource
