/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-11-10
 * @brief 
 */

#include "Shader.h"

#include "Logcat.h"
#include "slang-com-helper.h"
#include "slang-com-ptr.h"
#include "slang.h"

#include "Resource.Shader.generated.h"

GENERATED_SOURCE()

bool resource::Shader::IsValid() const
{
    return loaded_;
}

void resource::Shader::Load()
{
    if (loaded_) return;
    using namespace slang;
    Slang::ComPtr<IGlobalSession> global_session;
    createGlobalSession(global_session.writeRef());

    SessionDesc             session_desc;
    Slang::ComPtr<ISession> session;
    global_session->createSession(session_desc, session.writeRef());

    TargetDesc target_desc;
    target_desc.format       = SLANG_HLSL;
    session_desc.targets     = &target_desc;
    session_desc.targetCount = 1;
    slangCreate
    Slang::ComPtr<IBlob> diagnostics;
    core::String p = path_.GetAbsolutePath();
    IModule*             module = session->loadModule(p.Data(), diagnostics.writeRef());
    if (diagnostics)
    {
        LOGGER.Error(logcat::Resource, "Failed to load shader: {}, {}", GetRelativePath(), (const char*)diagnostics->getBufferPointer());
    }
}
