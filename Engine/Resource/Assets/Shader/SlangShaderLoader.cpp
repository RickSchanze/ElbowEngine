//
// Created by Echo on 24-12-25.
//

#include "SlangShaderLoader.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Resource/Config/ResourceConfig.h"
#include "Resource/Logcat.h"
#include "Shader.h"

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

using namespace resource;
using namespace slang;


SlangShaderLoader::SlangShaderLoader()
{
    createGlobalSession(global_session_.writeRef());
}

void SlangShaderLoader::Load(core::StringView path, Shader& shader) const
{
    std::lock_guard lock(mutex_);
    PROFILE_SCOPE_AUTO;
    SessionDesc session_desc{};

    auto*                    res_cfg      = core::GetConfig<ResourceConfig>();
    const auto&              search_paths = res_cfg->GetShaderSearchPath();
    core::Array<const char*> search_path_cstrs =
        search_paths | ranges::views::transform([](const core::String& path) { return *path; }) | ranges::to<core::Array<const char*>>();
    search_path_cstrs.push_back("");
    session_desc.searchPaths = search_path_cstrs.data();

    TargetDesc target_desc{};
    target_desc.format       = SLANG_SPIRV;
    target_desc.profile      = global_session_->findProfile("glsl_450");
    session_desc.targets     = &target_desc;
    session_desc.targetCount = 1;

    Slang::ComPtr<ISession> session;
    global_session_->createSession(session_desc, session.writeRef());

    Slang::ComPtr<IBlob>   diagnostics;
    Slang::ComPtr<IModule> module;
    module = session->loadModule(*path, diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    if (module)
    {
        shader.module_ = module;
    }
}

void SlangShaderLoader::DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob>& diag)
{
    if (diag)
    {
        LOGGER.Error(logcat::Resource, "Shader Load Error: {}", static_cast<const char*>(diag->getBufferPointer()));
    }
}