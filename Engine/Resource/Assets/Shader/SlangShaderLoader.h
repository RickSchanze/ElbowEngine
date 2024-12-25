//
// Created by Echo on 24-12-25.
//

#pragma once
#include "Core/Base/EString.h"
#include "Core/CoreDef.h"
#include "Core/Profiler/ProfileMacro.h"
#include "slang-com-ptr.h"
#include "slang.h"

#include <mutex>

namespace resource
{
class Shader;

class SlangShaderLoader
{
public:
    SlangShaderLoader();

    void Load(core::StringView path, OUT Shader& shader) const;

    static void DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob>& diag);

protected:
    Slang::ComPtr<slang::IGlobalSession> global_session_;

    mutable DECLARE_TRACEABLE_MUTEX_AUTO(std::mutex, mutex_);
};
}   // namespace resource
