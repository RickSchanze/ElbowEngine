//
// Created by Echo on 2025/3/29.
//

#include "UiManager.hpp"

#include "Core/Config/ConfigManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Core/Serialization/YamlArchive.hpp"
#include "Func/Config/FuncConfig.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/World/WorldClock.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/VertexLayout.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Font/Font.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Texture/Sprite.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "Style.hpp"
#include "UiUtility.hpp"
#include "Widget/Widget.hpp"
#include "Widget/Window.hpp"

using namespace rhi;

void UIBufferWrite::WriteVertex(const UInt64 index, const Vertex_UI &in_vertex) const {
    Assert(index < vertex_count, "index out of range");
    vertex_buffer[index] = in_vertex;
}

void UIBufferWrite::WriteIndex(UInt64 index, UInt32 value) const {
    Assert(index < index_count, "index out of range");
    index_buffer[index] = value;
}

void UIBufferWrite::AddQuad(rhi::Vertex_UI &left_top, rhi::Vertex_UI &right_top, rhi::Vertex_UI &left_bottom, rhi::Vertex_UI &right_bottom) {
    // TODO: 当顶点值小于1时表示相对位置, 大于1时表示绝对位置
    WriteVertex(vertex_cursor++, left_top);
    WriteVertex(vertex_cursor++, right_top);
    WriteVertex(vertex_cursor++, left_bottom);
    WriteVertex(vertex_cursor++, right_bottom);

    UInt64 index_end = vertex_offset + vertex_cursor;
    WriteIndex(index_cursor++, index_end - 2);
    WriteIndex(index_cursor++, index_end - 3);
    WriteIndex(index_cursor++, index_end - 4);
    WriteIndex(index_cursor++, index_end - 2);
    WriteIndex(index_cursor++, index_end - 1);
    WriteIndex(index_cursor++, index_end - 3);
}

void UIBufferWrite::FillQuadUV(const Rect2Df uv_range, rhi::Vertex_UI &left_top, rhi::Vertex_UI &right_top, rhi::Vertex_UI &left_bottom,
                               rhi::Vertex_UI &right_bottom) {
    left_top.uv.x = uv_range.pos.x;
    left_top.uv.y = uv_range.pos.y;
    right_top.uv.x = uv_range.pos.x + uv_range.size.x;
    right_top.uv.y = uv_range.pos.y;
    left_bottom.uv.x = uv_range.pos.x;
    left_bottom.uv.y = uv_range.pos.y + uv_range.size.y;
    right_bottom.uv.x = uv_range.pos.x + uv_range.size.x;
    right_bottom.uv.y = uv_range.pos.y + uv_range.size.y;
}

void UIBufferWrite::SetQuadColor(Color color, rhi::Vertex_UI &left_top, rhi::Vertex_UI &right_top, rhi::Vertex_UI &left_bottom,
                                 rhi::Vertex_UI &right_bottom) {
    left_top.color = color;
    right_top.color = color;
    right_bottom.color = color;
    left_bottom.color = color;
}

void UIBufferWrite::AddQuad(Rect2Df rect, Rect2Df uv, Color color) {
    auto quad = UIUtility::ExtractVertexFromRect2D(rect);
    SetQuadColor(color, quad.left_top, quad.right_top, quad.left_bottom, quad.right_bottom);
    FillQuadUV(uv, quad.left_top, quad.right_top, quad.left_bottom, quad.right_bottom);
    AddQuad(quad.left_top, quad.right_top, quad.left_bottom, quad.right_bottom);
}

UIVertexIndexManager::UIVertexIndexManager(UInt64 preserved_vertex_cnt, UInt64 preserved_index_cnt) {
    ProfileScope _(__func__);
    UInt64 vertex_buffer_size = preserved_vertex_cnt * sizeof(rhi::Vertex_UI);
    BufferDesc desc{vertex_buffer_size, BUB_VertexBuffer, BMPB_HostVisible | BMPB_HostCoherent};
    vertex_buffer = GetGfxContextRef().CreateBuffer(desc, "UIVertexBuffer");
    vertex_data = reinterpret_cast<Vertex_UI *>(vertex_buffer->BeginWrite());

    UInt64 index_buffer_size = preserved_index_cnt * sizeof(UInt32);
    desc = {index_buffer_size, BUB_IndexBuffer, BMPB_HostVisible | BMPB_HostCoherent};
    index_buffer = GetGfxContextRef().CreateBuffer(desc, "UIIndexBuffer");
    index_data = reinterpret_cast<UInt32 *>(index_buffer->BeginWrite());
}

UIVertexIndexManager::~UIVertexIndexManager() {
    vertex_buffer->EndWrite();
    vertex_buffer = nullptr;

    index_buffer->EndWrite();
    index_buffer = nullptr;
}

WidgetVertexIndexBufferInfo *UIVertexIndexManager::RequestWidgetBufferInfo(ObjectHandle handle) {
    if (widget_buffer_map.Contains(handle)) {
        return &widget_buffer_map[handle];
    }
    return nullptr;
}

void UIManager::Startup() {
    const auto *cfg = GetConfig<FuncConfig>();
    buffer_manager_ = MakeUnique<UIVertexIndexManager>(cfg->GetPreservedUIVertexCount(), cfg->GetPreservedUIIndexCount());

    TickEvents::Evt_PostInputTick.AddBind(&dispatcher_, &UIEventDispatcher::ProcessInputEvent);

    auto style_file_path = cfg->GetUIStylePath();
    current_style_ = MakeUnique<Style>();
    if (Path::IsExist(style_file_path)) {
        if (auto op_style_str = File::ReadAllText(style_file_path)) {
            YamlArchive archive;
            archive.Deserialize(*op_style_str, current_style_.Get(), Style::GetStaticType());
        } else {
            VLOG_ERROR("读取UI样式文件", *style_file_path, "失败");
        }
    } else {
        VLOG_ERROR("UI样式文件", *style_file_path, "不存在");
    }

    auto default_shader = reinterpret_cast<Shader *>(AssetDataBase::Load("Assets/Shader/UIDefault.slang"));
    Material *default_ui = ObjectManager::CreateNewObject<Material>();
    default_ui->SetName("DefaultUIFontMaterial");
    default_ui->SetShader(default_shader);
    default_ui_mat_ = default_ui;
    auto default_ui_icon_atlas = reinterpret_cast<Texture2D *>(AssetDataBase::Load("Assets/Texture/UIAtlas.png"));
    default_ui_mat_->SetTexture2D("atlas", default_ui_icon_atlas);

    Material *default_ui_font = ObjectManager::CreateNewObject<Material>();
    default_ui_font->SetName("DefaultUIFontMaterial");
    default_ui_font->SetShader(default_shader);
    default_ui_font_mat_ = default_ui_font;
    Font *default_font = Font::GetDefaultFont();
    default_ui_font_mat_->SetTexture2D("atlas", default_font->GetFontAtlas());

    post_tick_handle_ = TickEvents::Evt_WorldPostTick.AddBind(&UIManager::PerformRebuildPass);
}

void UIManager::Shutdown() {
    buffer_manager_ = nullptr;
    TickEvents::Evt_WorldPreTick.RemoveBind(post_tick_handle_);
}

UIBufferWrite UIManager::RequestWriteData(ObjectHandle handle, UInt64 vertex_count, UInt64 index_count) {
    auto &self = GetByRef();
    return self.buffer_manager_->RequestBufferWrite(handle, vertex_count, index_count);
}

UIBufferWrite UIManager::RequestWriteData(Widget *w, UInt64 vertex_count, UInt64 index_count) {
    return RequestWriteData(w->GetHandle(), vertex_count, index_count);
}

UIBufferWrite UIManager::RequestWriteData(Widget *w) {
    auto &self = GetByRef();
    return self.buffer_manager_->RequestBufferWrite(w->GetHandle());
}

WidgetVertexIndexBufferInfo *UIManager::GetWidgetBufferInfo(ObjectHandle handle) {
    auto &self = GetByRef();
    return self.buffer_manager_->RequestWidgetBufferInfo(handle);
}

WidgetVertexIndexBufferInfo *UIManager::GetWidgetBufferInfo(Widget *w) { return w == nullptr ? nullptr : GetWidgetBufferInfo(w->GetHandle()); }

Style &UIManager::GetCurrentStyle() {
    auto &self = GetByRef();
    return *self.current_style_;
}

void UIManager::PerformRebuildPass(const MilliSeconds &) {
    auto &self = GetByRef();
    self.rebuilding_ = true;
    for (auto &w: self.windows_) {
        w->RebuildHierarchy();
    }
    self.rebuilding_ = false;
}

void UIManager::PerformGenerateRenderCommandsPass(rhi::CommandBuffer &cmd) {
    auto &self = GetByRef();
    cmd.Enqueue<Cmd_BindIndexBuffer>(self.buffer_manager_->index_buffer.get());
    cmd.Enqueue<Cmd_BindVertexBuffer>(self.buffer_manager_->vertex_buffer.get());
    for (auto &w: self.windows_) {
        w->Draw(cmd);
    }
    cmd.Execute("Draw UI");
}

Material *UIManager::GetDefaultUIFontMaterial() {
    auto &self = GetByRef();
    return self.default_ui_font_mat_;
}

Material *UIManager::GetDefaultUIMaterial() {
    auto &self = GetByRef();
    return self.default_ui_mat_;
}

void UIManager::RegisterWindow(Window *w) {
    auto &self = GetByRef();
    self.windows_.AddUnique(w);
}

void UIManager::UnRegisterWindow(Window *w) {
    auto &self = GetByRef();
    self.windows_.Remove(w);
}

Rect2Df UIManager::GetIconAtlasUV(StringView name) {
    auto &self = GetByRef();
    static Texture2D *tex = AssetDataBase::Load<Texture2D>("Assets/Texture/UIAtlas.png");
    return Sprite::GetUVRange(tex, name);
}

bool UIManager::IsRebuilding() {
    auto& self = GetByRef();
    return self.rebuilding_;
}

const Array<Window *> &UIManager::GetWindows() {
    auto &self = GetByRef();
    return self.windows_;
}

Float ApplyGlobalUIScale(Float value) { return value * UIManager::GetCurrentStyle().global_scale; }

struct OccupiedMemory {
    UInt64 offset;
    UInt64 size;
};

UIBufferWrite UIVertexIndexManager::RequestBufferWrite(ObjectHandle handle, UInt64 vertex_count, UInt64 index_count) {
    ProfileScope _(__func__);
    UIBufferWrite write_data{};
    if (widget_buffer_map.Contains(handle)) {
        const auto &buffer = widget_buffer_map[handle];
        if (buffer.index_count == index_count) {
            write_data.index_buffer = index_data + buffer.index_offset;
            write_data.index_count = index_count;
            write_data.index_offset = buffer.index_offset;
            write_data.index_cursor = 0;
        }
        if (buffer.vertex_count == vertex_count) {
            write_data.vertex_buffer = vertex_data + buffer.vertex_offset;
            write_data.vertex_count = vertex_count;
            write_data.vertex_offset = buffer.vertex_offset;
            write_data.vertex_cursor = 0;
        }
    }
    if (write_data.index_buffer == nullptr) {
        Array<OccupiedMemory> sorted_index;
        sorted_index.Reserve(widget_buffer_map.Count());
        for (const auto &value: widget_buffer_map | range::view::Values) {
            sorted_index.Add({value.index_offset, value.index_count});
        }
        if (sorted_index.Count() == 0) {
            write_data.index_buffer = index_data;
            write_data.index_count = index_count;
            write_data.index_offset = 0;
            write_data.index_cursor = 0;
        } else {
            sorted_index.SortInplace([](const OccupiedMemory &lhs, const OccupiedMemory &rhs) { return lhs.offset < rhs.offset; });
            UInt64 cursor = 0;
            for (const auto &value: sorted_index) {
                if (value.offset - cursor >= index_count) {
                    write_data.index_buffer = index_data + cursor;
                    write_data.index_count = index_count;
                    write_data.index_offset = cursor;
                    write_data.index_cursor = 0;
                    break;
                }
                cursor = value.offset + value.size;
            }
            if (write_data.index_buffer == nullptr) {
                write_data.index_buffer = index_data + cursor;
                write_data.index_count = index_count;
                write_data.index_offset = cursor;
                write_data.index_cursor = 0;
            }
        }
    }
    if (write_data.vertex_buffer == nullptr) {
        Array<OccupiedMemory> sorted_vertex;
        sorted_vertex.Reserve(widget_buffer_map.Count());
        for (const auto &value: widget_buffer_map | range::view::Values) {
            sorted_vertex.Add({value.vertex_offset, value.vertex_count});
        }
        sorted_vertex.SortInplace([](const OccupiedMemory &lhs, const OccupiedMemory &rhs) { return lhs.offset < rhs.offset; });
        if (sorted_vertex.Count() == 0) {
            write_data.vertex_buffer = vertex_data;
            write_data.vertex_count = vertex_count;
            write_data.vertex_offset = 0;
            write_data.vertex_cursor = 0;
        } else {
            UInt64 cursor = 0;
            for (const auto &value: sorted_vertex) {
                if (value.offset - cursor >= vertex_count) {
                    write_data.vertex_buffer = vertex_data + cursor;
                    write_data.vertex_count = vertex_count;
                    write_data.vertex_offset = cursor;
                    write_data.vertex_cursor = 0;
                    break;
                }
                cursor = value.offset + value.size;
            }
            if (write_data.vertex_buffer == nullptr) {
                write_data.vertex_buffer = vertex_data + cursor;
                write_data.vertex_count = vertex_count;
                write_data.vertex_offset = cursor;
                write_data.vertex_cursor = 0;
            }
        }
    }
    // 更新map
    WidgetVertexIndexBufferInfo info;
    info.index_offset = write_data.index_buffer - index_data;
    info.index_count = write_data.index_count;
    info.vertex_offset = write_data.vertex_buffer - vertex_data;
    info.vertex_count = write_data.vertex_count;
    widget_buffer_map[handle] = info;
    return write_data;
}

UIBufferWrite UIVertexIndexManager::RequestBufferWrite(ObjectHandle handle) {
    if (widget_buffer_map.Contains(handle)) {
        const auto &buffer = widget_buffer_map[handle];
        UIBufferWrite write_data{};
        write_data.index_buffer = index_data + buffer.index_offset;
        write_data.index_count = buffer.index_count;
        write_data.index_offset = buffer.index_offset;
        write_data.index_cursor = 0;
        write_data.vertex_buffer = vertex_data + buffer.vertex_offset;
        write_data.vertex_count = buffer.vertex_count;
        write_data.vertex_offset = buffer.vertex_offset;
        write_data.vertex_cursor = 0;
        return write_data;
    }
    return {};
}

void UIVertexIndexManager::ReleaseBuffer(ObjectHandle handle) { widget_buffer_map.Remove(handle); }
