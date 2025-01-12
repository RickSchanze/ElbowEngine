//
// Created by Echo on 25-1-12.
//

#pragma once
#include "Core/Singleton/MManager.h"
#include "Func/World/ITick.h"

namespace func
{
class CameraComponent;
}
namespace func
{
class Camera : public core::Manager<Camera>, public ITick
{
public:
    CameraComponent* GetActive() const { return active_; }

    void SetActive(CameraComponent* camera);

    core::ManagerLevel GetLevel() const override { return core::ManagerLevel::Fourth; }
    core::StringView GetName() const override { return "Camera"; }

    void Tick(Millisecond delta_time) override;

private:
    CameraComponent* active_ = nullptr;
};
}   // namespace func
