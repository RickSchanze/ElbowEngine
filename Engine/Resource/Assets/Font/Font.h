//
// Created by Echo on 25-1-30.
//

#pragma once
#include "Resource/Assets/Asset.h"

#include GEN_HEADER("Resource.Font.generated.h")

namespace resource {

enum class FontRenderMethod {
  SDF,
  Texture,
};

class CLASS() Font : public Asset {
public:
  [[nodiscard]] AssetType GetAssetType() const override { return resource::AssetType::Font; }

  void PerformLoad() override;
  void PerformUnload() override;

  Int16 GetFontSize() const { return font_size_; }
  /**
   * 设置字体大小, 这会生成新的FontAtlas
   */
  void SetFontSize(Int16 new_size);

  FontRenderMethod GetRenderMethod() const { return render_method_; }
  core::StringView GetAssetPath() const { return path_; }

private:
  Int16 font_size_ = 16;                                       // 静态字体大小
  FontRenderMethod render_method_ = FontRenderMethod::Texture; // 默认不使用SDF
  core::String path_;                                          // 资源路径
};
}
