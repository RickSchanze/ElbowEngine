//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"
#include "Shader.hpp"
#include "slang-com-ptr.h"


static void ParseAnnotations(StringView path, Int32 (&output)[static_cast<Int32>(ShaderAnnotation::Count)], String &name);

class SlangShaderLoader {
public:
    static void Load(StringView path, OUT Shader &shader);

    static void DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob> &diag);

protected:
    static inline Slang::ComPtr<slang::IGlobalSession> global_session_;

    static inline Mutex mutex_;
};
