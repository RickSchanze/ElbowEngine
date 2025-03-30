//
// Created by Echo on 2025/3/30.
//

#include "VerticalLayout.hpp"

IMPL_REFLECTED(VerticalLayout) {
    return Type::Create<VerticalLayout>("VerticalLayout") | refl_helper::AddParents<Layout>() |
           refl_helper::AddField("children", &ThisClass::children_) | refl_helper::AddField("layout", &ThisClass::layout_) |
           refl_helper::AddField("space", &ThisClass::space);
}

VerticalLayout::VerticalLayout() {
    SetDisplayName("垂直布局");
    SetName("VerticalLayout");
    layout_ = EVerticalLayout::Top;
    ui_rect_.size = {};
}

Vector2f VerticalLayout::GetRebuildRequiredSize() const { return ui_rect_.size; }

void VerticalLayout::Rebuild() {
    Super::Rebuild();
    // Rebuild时父widget已经把尺寸设置好
    Vector2f cursor = {ui_rect_.pos.x, ui_rect_.pos.y + ui_rect_.size.y};
    for (Widget *w: children_) {
        // TODO: 根据枚举layout_来布局
        auto size = w->GetRebuildRequiredSize();
        w->SetLocation({cursor.x, cursor.y - size.y});
        w->SetSize(size);
        w->Rebuild();
        cursor.y -= (size.y + space);
        // TODO: 多余的裁剪掉
    }
    ui_rect_.size = {};
}

void VerticalLayout::RebuildHierarchy() {
    for (Widget *w: children_) {
        if (w) {
            if (w->IsRebuildDirty()) {
                w->Rebuild();
                continue;
            }
            if (w->GetType()->IsDerivedFrom(Layout::GetStaticType())) {
                static_cast<Layout *>(w)->RebuildHierarchy();
            } else {
                if (w->IsRebuildDirty()) {
                    w->Rebuild();
                }
            }
        }
    }
}

void VerticalLayout::Draw(rhi::CommandBuffer &cmd) {
    for (Widget *w: children_) {
        w->Draw(cmd);
    }
}

void VerticalLayout::AddChild(Widget *w) {
    if (w != nullptr) {
        children_.Add(w);
        SetRebuildDirty();
    }
}

void VerticalLayout::RemoveChild(Widget *w) {
    if (w != nullptr) {
        children_.RemoveIf([handle = w->GetHandle()](const ObjectPtrBase &ptr) { return ptr.GetHandle() == handle; });
        // TODO: 这里起始不需要整个Rebuild
        SetRebuildDirty();
    }
}

void VerticalLayout::RemoveChild(Int32 i) {
    Assert(i >= 0 && i < children_.Count(), "index out of range");
    children_.RemoveAt(i);
    // TODO: 这里起始不需要整个Rebuild
    SetRebuildDirty();
}
