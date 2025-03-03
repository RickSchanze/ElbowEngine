//
// Created by Echo on 25-1-30.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Object/Object.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include GEN_HEADER("Resource.FontMeta.generated.h")

namespace resource {
struct STRUCT(SQLTable = "Font") FontMeta {
  GENERATED_STRUCT(FontMeta)
public:
  PROPERTY(SQLAttr = "(PrimaryKey, AutoIncrement)")
  Int32 id{};

  PROPERTY()
  core::ObjectHandle object_handle{};

  PROPERTY()
  core::String path;

  PROPERTY()
  Int32 font_atlas_width = 2048;

  PROPERTY()
  Int32 font_atlas_height = 2048;

  PROPERTY()
  Int32 font_size = 16;

  PROPERTY()
  core::String font_charset_file = "Assets/Font/Charset.txt";
};
} // namespace resource