//
// Created by Echo on 25-1-15.
//

#pragma once

#include "Core/Object/Object.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include "Platform/RHI/Enums.h"
#include GEN_HEADER("Resource.Texture2DMeta.generated.h")

namespace resource {
class CLASS(SQLTable = "Texture2D") Texture2DMeta {
  GENERATED_CLASS(Texture2DMeta)
public:
  PROPERTY(SQLAttr = "(PrimaryKey, AutoIncrement)")
  int32_t id{};

  PROPERTY()
  core::ObjectHandle object_handle{};

  PROPERTY()
  core::String path;

  PROPERTY()
  platform::rhi::Format format = platform::rhi::Format::R8G8B8A8_SRGB;

  // 这个Texture2D包含的所有Sprite
  PROPERTY()
  core::String sprites_string;

  // 动态创建的Texture2D的参数
  bool dynamic = false;
  UInt32 width = 0;
  UInt32 height = 0;
};
} // namespace resource
