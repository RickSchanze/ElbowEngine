//
// Created by Echo on 24-12-25.
//

#pragma once

#include "Resource/Assets/Asset.h"

#include "slang-com-ptr.h"

namespace resource
{
/**
 * 对应一个slang文件
 * 一个slang可以通过[shader]语法声明多个shader
 * 不能有相同的stage
 */
class Shader : public Asset
{
public:
    friend class SlangShaderLoader;
    void PerformLoad() override;

    [[nodiscard]] bool IsLoaded() const override;

protected:
    Slang::ComPtr<slang::IModule> module_;
};
}
