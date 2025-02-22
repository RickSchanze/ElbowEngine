//
// Created by Echo on 25-2-9.
//

#pragma once
#include "Widget.h"

#include "Core/Object/ObjectPtr.h"
#include "Resource/Assets/Texture/Sprite.h"

#include "Func.Panel.generated.h"

namespace resource {
class Material;
}
namespace resource {
struct Sprite;
}
namespace func::ui::widget {
/**
 * Panel是一个覆盖整个TargetRect的Widget
 */
class CLASS() Panel : public Widget {
  GENERATED_CLASS(Panel)
public:
  Panel();

  void Rebuild(core::Rect2DI target_rect, core::Array<platform::rhi::Vertex_UI> &vertex_buffer,
               core::Array<UInt32> &index_buffer) override;
  void Draw(platform::rhi::CommandBuffer &cmd) override;
  core::Rect2DI GetBoundingRect() override;

  Panel& SetMaterial(resource::Material* mat);
  resource::Material* GetMaterial();

  void SetWidth(UInt32 w);
  void SetHeight(UInt32 h);

protected:
  [[nodiscard]] core::Rect2DI GetDrawRect(const core::Rect2DI& target) const;

  /// 背景图像, 应是一个TextureAtlas里的一部分
  PROPERTY()
  resource::Sprite sprite_;

  /// 附加的颜色
  PROPERTY()
  core::Color color_ = core::Color::White();

  /// 附加的材质
  PROPERTY()
  core::ObjectPtr<resource::Material> material_;

  PROPERTY()
  core::Vector2I position_ = {0, 0};

  PROPERTY()
  core::Vector2I size_ = {100, 100};

  UInt64 index_offset_ = 0;
};
} // namespace func::ui::widget
