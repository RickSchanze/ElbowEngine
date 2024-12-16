//
// Created by Echo on 24-12-16.
//

#pragma once
#include "Core/Core.h"

#include GEN_HEADER("Resource.MeshMeta.generated.h")

namespace core::resource
{
class CLASS(SQLTable) MeshMeta
{
    GENERATED_CLASS(MeshMeta)

public:
    PROPERTY(SQLAttr = "(PrimaryKey, AutoIncrement)")
    int32_t id;
};
}   // namespace core::resource
