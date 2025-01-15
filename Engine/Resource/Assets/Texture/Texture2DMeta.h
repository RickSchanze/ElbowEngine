//
// Created by Echo on 25-1-15.
//

#pragma once

#include "Core/Object/Object.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include GEN_HEADER("Resource.Texture2DMeta.generated.h")

namespace resource
{
class CLASS(SQLTable = "Texture2D") Texture2DMeta
{
    GENERATED_CLASS(Texture2DMeta)
public:
    PROPERTY(SQLAttr = "(PrimaryKey, AutoIncrement)")
    int32_t id{};

    PROPERTY()
    core::ObjectHandle object_handle{};

    PROPERTY()
    core::String path;
};
}   // namespace resource
