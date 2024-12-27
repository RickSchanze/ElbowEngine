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

    static void Load(core::StringView path, OUT Shader& shader);

    static void DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob>& diag);

protected:
    static inline Slang::ComPtr<slang::IGlobalSession> global_session_;

    static inline DECLARE_TRACEABLE_MUTEX_AUTO(std::mutex, mutex_);
};
}   // namespace resource
