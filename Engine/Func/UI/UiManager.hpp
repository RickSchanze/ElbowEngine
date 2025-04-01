//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Math/Types.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Core/Object/Object.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "UIEventDispatcher.hpp"


class Material;
class Window;
class Style;
class Widget;
namespace rhi {
    struct Vertex_UI;
}

struct WidgetVertexIndexBufferInfo {
    UInt64 index_count{};
    UInt64 index_offset{};
    UInt64 vertex_count{};
    UInt64 vertex_offset{};
};

struct UIBufferWrite {
    friend struct UIVertexIndexManager;
    rhi::Vertex_UI *vertex_buffer; // 当前write的起始指针
    UInt64 vertex_count{}; // 当前write的最大顶点数
    UInt64 vertex_offset; // 当前write相当于原始数据的偏移
    UInt64 vertex_cursor; // 当使用AddQuad等函数时, 需要使用此值记录当前加到多少了

    UInt32 *index_buffer; // 同上
    UInt64 index_count{};
    UInt64 index_offset;
    UInt64 index_cursor;

    void WriteVertex(UInt64 index, const rhi::Vertex_UI &in_vertex) const;
    void WriteIndex(UInt64 index, UInt32 value) const;

    void AddQuad(rhi::Vertex_UI &left_top, rhi::Vertex_UI &right_top, rhi::Vertex_UI &left_bottom, rhi::Vertex_UI &right_bottom);
    static void FillQuadUV(Rect2Df uv_range, rhi::Vertex_UI &left_top, rhi::Vertex_UI &right_top, rhi::Vertex_UI &left_bottom,
                           rhi::Vertex_UI &right_bottom);
    static void SetQuadColor(Color color, rhi::Vertex_UI &left_top, rhi::Vertex_UI &right_top, rhi::Vertex_UI &left_bottom,
                             rhi::Vertex_UI &right_bottom);

    void AddQuad(Rect2Df rect, Rect2Df uv, Color color);

private:
    UIBufferWrite() = default;
};

struct UIVertexIndexManager {
    SharedPtr<rhi::Buffer> vertex_buffer;
    SharedPtr<rhi::Buffer> index_buffer;
    rhi::Vertex_UI *vertex_data{};
    UInt32 *index_data{};

    Map<ObjectHandle, WidgetVertexIndexBufferInfo> widget_buffer_map;

    // 为顶点和索引缓冲预分配内存
    UIVertexIndexManager(UInt64 preserved_vertex_cnt, UInt64 preserved_index_cnt);

    ~UIVertexIndexManager();

    UIBufferWrite RequestBufferWrite(ObjectHandle handle, UInt64 vertex_count, UInt64 index_count);
    UIBufferWrite RequestBufferWrite(ObjectHandle handle);

    void ReleaseBuffer(ObjectHandle);

    WidgetVertexIndexBufferInfo *RequestWidgetBufferInfo(ObjectHandle handle);
};

class UIManager : public Manager<UIManager> {
    friend class UIEventDispatcher;

public:
    StringView GetName() const override { return "UIManager"; }
    Float GetLevel() const override { return 13.5f; }

    void Startup() override;
    void Shutdown() override;

    static UIBufferWrite RequestWriteData(ObjectHandle handle, UInt64 vertex_count, UInt64 index_count);
    static UIBufferWrite RequestWriteData(Widget *w, UInt64 vertex_count, UInt64 index_count);
    static UIBufferWrite RequestWriteData(Widget *w);
    static WidgetVertexIndexBufferInfo *GetWidgetBufferInfo(ObjectHandle handle);
    static WidgetVertexIndexBufferInfo *GetWidgetBufferInfo(Widget *w);
    static Style &GetCurrentStyle();
    static void PerformRebuildPass(const MilliSeconds &);
    static void PerformGenerateRenderCommandsPass(rhi::CommandBuffer &cmd);
    static Material *GetDefaultUIFontMaterial();
    static Material *GetDefaultUIMaterial();
    static void RegisterWindow(Window *w);
    static void UnRegisterWindow(Window *w);
    static Rect2Df GetIconAtlasUV(StringView name);
    static bool IsRebuilding();

private:
    static const Array<Window *> &GetWindows();

    UniquePtr<UIVertexIndexManager> buffer_manager_;
    UniquePtr<Style> current_style_;
    Array<Window *> windows_;

    ObjectPtr<Material> default_ui_mat_ = nullptr;
    ObjectPtr<Material> default_ui_font_mat_ = nullptr;

    UInt64 post_tick_handle_ = 0;

    UIEventDispatcher dispatcher_;
    bool rebuilding_ = false;
};

Float ApplyGlobalUIScale(Float value);
