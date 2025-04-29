//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/TypeAlias.hpp"
#include "Func/World/ITick.hpp"


namespace RHI {
    class Buffer;
}
class CameraComponent;
struct CameraShaderData;


class Camera : public Manager<Camera>, public ITick {
public:
    CameraComponent *GetActive() const { return active_; }

    void SetActive(CameraComponent *camera);

    virtual Float GetLevel() const override { return 11; }
    virtual StringView GetName() const override
    {
        return "Camera";
    }

    virtual void Tick(MilliSeconds delta_time) override;

    static void UpdateViewBuffer(const CameraShaderData &data);
    static RHI::Buffer* GetViewBuffer()
    {
        return GetByRef().view_buffer_.get();
    }

    virtual void Startup() override;
    virtual void Shutdown() override;

private:
    CameraComponent *active_ = nullptr;

    // 全局摄像机视图缓存
    SharedPtr<RHI::Buffer> view_buffer_ = nullptr;
    UInt8 *mapped_view_buffer_memory_ = nullptr;
};
