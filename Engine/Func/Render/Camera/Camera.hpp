//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/TypeAlias.hpp"
#include "Func/World/ITick.hpp"


namespace rhi {
    class Buffer;
}
class CameraComponent;
struct CameraShaderData;


class Camera : public Manager<Camera>, public ITick {
public:
    [[nodiscard]] CameraComponent *GetActive() const { return active_; }

    void SetActive(CameraComponent *camera);

    [[nodiscard]] Float GetLevel() const override { return 11; }
    [[nodiscard]] StringView GetName() const override { return "Camera"; }

    void Tick(MilliSeconds delta_time) override;

    static void UpdateViewBuffer(const CameraShaderData &data);
    static rhi::Buffer *GetViewBuffer() { return GetByRef().view_buffer_.get(); }

    void Startup() override;
    void Shutdown() override;

private:
    CameraComponent *active_ = nullptr;

    // 全局摄像机视图缓存
    SharedPtr<rhi::Buffer> view_buffer_ = nullptr;
    UInt8 *mapped_view_buffer_memory_ = nullptr;
};
