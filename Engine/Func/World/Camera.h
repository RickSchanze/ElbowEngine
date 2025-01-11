//
// Created by Echo on 25-1-11.
//

#pragma once
#include "Actor.h"

#include GEN_HEADER("Func.Camera.generated.h")

namespace platform::rhi
{
class Buffer;
}
namespace func
{

class CLASS() Camera : public Actor
{
    GENERATED_CLASS(Camera)

private:
    // 此Buffer包括VP矩阵, 以及相机的各项参数
    core::SharedPtr<platform::rhi::Buffer> camera_buffer_;
};

}   // namespace func
