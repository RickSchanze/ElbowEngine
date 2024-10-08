/**
 * @file Camera.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once
#include "Component.h"
#include "CoreDef.h"
#include "Math/MathTypes.h"

#include "Camera.generated.h"

#undef near
#undef far

namespace function::comp
{

ECLASS()
class Camera : public TickableComponent
{
    GENERATED_BODY(Camera)
public:
    explicit Camera();

    void Tick() override;

    void BeginPlay() override;

    static inline Camera* Main = nullptr;

    Matrix4x4 GetViewMatrix() const;
    Matrix4x4 GetProjectionMatrix() const;

    void SetWindowFocused(bool focused);

public:
    EPROPERTY(Label = "背景颜色")
    Color background_color = Color(0.5f, 0.5f, 0.5f, 1.0f);

    // 沟槽的微软定义 #define near
    EPROPERTY(Label = "近平面")
    float near_plane = 0.1f;

    EPROPERTY(Label = "远平面")
    float far_plane = 1000.f;

    EPROPERTY(Label = "绘制天空盒")
    bool draw_skybox = true;

    EPROPERTY(Label = "绘制网格线")
    bool draw_grid = true;

protected:
    void HandleInput();

    EPROPERTY(Name = movement_speed, Label = "摄像机移动速度")
    float movement_speed_ = 10.f;

    EPROPERTY(Name = enable_input, Label = "启用输入")
    bool enable_input_ = true;

    EPROPERTY(Name = mouse_sensitity, Label = "鼠标灵敏度")
    float mouse_sensitivity_ = 45.f;


    bool focused_ = false;
};

}

// }
