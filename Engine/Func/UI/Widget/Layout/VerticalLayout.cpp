//
// Created by Echo on 2025/3/30.
//

#include "VerticalLayout.hpp"

#include "Core/Profile.hpp"
#include "Func/UI/UiManager.hpp"
#include "Func/UI/UiUtility.hpp"

IMPL_REFLECTED(VerticalLayout) {
    return Type::Create<VerticalLayout>("VerticalLayout") | refl_helper::AddParents<Layout>() |
           refl_helper::AddField("children", &ThisClass::children_) | refl_helper::AddField("layout", &ThisClass::layout_) |
           refl_helper::AddField("space", &ThisClass::space);
}

VerticalLayout::VerticalLayout() {
    ProfileScope _("VerticalLayout::VerticalLayout");
    SetDisplayName("垂直布局");
    SetName("VerticalLayout");
    layout_ = EVerticalLayout::Top;
    rel_rect_ = {};
}

void VerticalLayout::OnMouseMove(Vector2f old, Vector2f now) {
    ProfileScope _(__func__);
    for (Widget *w: children_) {
        Rect2Df w_rect = w->GetAbsoluteRect();
        if (UIUtility::IsRectContainsPos(w_rect, now)) {
            if (w != entered_widget_) {
                if (entered_widget_) {
                    entered_widget_->OnMouseLeave();
                }
                w->OnMouseEnter();
                entered_widget_ = w;
            } else {
                w->OnMouseMove(old, now);
            }
            return;
        }
    }
    if (entered_widget_) {
        entered_widget_->OnMouseLeave();
        entered_widget_ = nullptr;
    }
}

void VerticalLayout::OnMouseButtonReleased(MouseButton button, Vector2f pos) {
    for (Widget *w: children_) {
        Rect2Df w_rect = w->GetAbsoluteRect();
        if (UIUtility::IsRectContainsPos(w_rect, pos)) {
            w->OnMouseButtonReleased(button, pos);
            break;
        }
    }
}

void VerticalLayout::OnMouseButtonPressed(MouseButton button, Vector2f pos) {
    for (Widget *w: children_) {
        Rect2Df w_rect = w->GetAbsoluteRect();
        if (UIUtility::IsRectContainsPos(w_rect, pos)) {
            w->OnMouseButtonPressed(button, pos);
            break;
        }
    }
}

Vector2f VerticalLayout::GetRebuildRequiredSize() const { return rel_rect_.size; }

void VerticalLayout::Rebuild() {
    ProfileScope _(__func__);
    Super::Rebuild();
    // Rebuild时父widget已经把尺寸设置好
    Vector2f cursor = {abs_rect_.pos.x, abs_rect_.pos.y + abs_rect_.size.y - ApplyGlobalUIScale(5.f)};
    for (Widget *w: children_) {
        // TODO: 根据枚举layout_来布局
        auto size = UIUtility::GetRebuildSize(w);
        size.x = size.x == 0 ? abs_rect_.size.x : size.x;
        w->SetAbsoluteSize(size);
        if (size.y == 0) {
            VLOG_WARN("Widget ", *w->GetName(), " 在VerticalLayout的size.y = 0, 将被忽略");
            continue;
        }
        w->SetAbsoluteLocation(w->GetRelativeLocation() + Vector2f{cursor.x, cursor.y - size.y});
        w->Rebuild();
        cursor.y -= (size.y + space + w->GetRelativeLocation().y);
        // TODO: 多余的裁剪掉
    }
}

void VerticalLayout::RebuildHierarchy() {
    ProfileScope _(__func__);
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
    ProfileScope _(__func__);
    for (Widget *w: children_) {
        w->Draw(cmd);
    }
}

void VerticalLayout::AddChild(Widget *w) {
    ProfileScope _(__func__);
    if (w != nullptr) {
        children_.AddUnique(w);
        SetRebuildDirty();
    }
}

void VerticalLayout::RemoveChild(Widget *w) {
    ProfileScope _(__func__);
    if (w != nullptr) {
        children_.RemoveIf([handle = w->GetHandle()](const ObjectPtrBase &ptr) { return ptr.GetHandle() == handle; });
        // TODO: 这里起始不需要整个Rebuild
        SetRebuildDirty();
    }
}

void VerticalLayout::RemoveChild(Int32 i) {
    ProfileScope _(__func__);
    Assert(i >= 0 && i < children_.Count(), "index out of range");
    children_.RemoveAt(i);
    // TODO: 这里起始不需要整个Rebuild
    SetRebuildDirty();
}
