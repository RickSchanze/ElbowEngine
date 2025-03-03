//
// Created by Echo on 25-1-16.
//

#pragma once
#include "Core/Object/Object.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include GEN_HEADER("Resource.MaterialMeta.generated.h")

namespace resource {
struct STRUCT(SQLTable = "Material") MaterialMeta {
  GENERATED_STRUCT(MaterialMeta)

  PROPERTY(SQLAttr = "(PrimaryKey, AutoIncrement)")
  Int32 id{};

  PROPERTY()
  core::ObjectHandle object_handle{};

  PROPERTY()
  core::String path;
};
} // namespace resource
