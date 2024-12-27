//
// Created by Echo on 24-12-25.
//

#include "SlangShaderLoader.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Resource/Config/ResourceConfig.h"
#include "Resource/Logcat.h"
#include "Shader.h"
#include "slang.h"

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

using namespace resource;
using namespace slang;
using namespace platform::rhi;

#define VERIFY_SLANG_RESULT(result)                                 \
    {                                                               \
        SlangResult res = result;                                   \
        if (SLANG_FAILED(res))                                      \
        {                                                           \
            LOGGER.Error(logcat::Resource, "Slang Error: {}", res); \
            return;                                                 \
        }                                                           \
    }


SlangShaderLoader::SlangShaderLoader()
{

}

void SlangShaderLoader::Load(core::StringView path, Shader& shader)
{
    std::lock_guard lock(mutex_);
    PROFILE_SCOPE_AUTO;
    SessionDesc session_desc{};
    if (!global_session_)
    {
        createGlobalSession(global_session_.writeRef());
    }
    auto*                    res_cfg      = core::GetConfig<ResourceConfig>();
    const auto&              search_paths = res_cfg->GetShaderSearchPath();
    core::Array<const char*> search_path_cstrs =
        search_paths | ranges::views::transform([](const core::String& path) { return *path; }) | ranges::to<core::Array<const char*>>();
    search_path_cstrs.push_back("");
    session_desc.searchPaths = search_path_cstrs.data();

    TargetDesc target_desc[2]{};
    target_desc[0].format  = SLANG_SPIRV;
    target_desc[0].profile = global_session_->findProfile("glsl_460");

    target_desc[1].format  = SLANG_GLSL;
    target_desc[1].profile = global_session_->findProfile("glsl_460");

    session_desc.targets     = target_desc;
    session_desc.targetCount = 2;

    Slang::ComPtr<ISession> session;
    global_session_->createSession(session_desc, session.writeRef());

    Slang::ComPtr<IBlob>   diagnostics;
    Slang::ComPtr<IModule> module;
    module = session->loadModule(*path, diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    if (!module) return;

    core::Array<Slang::ComPtr<IComponentType>> linking_programs;
    Slang::ComPtr<IEntryPoint>                 vert;
    Slang::ComPtr<IEntryPoint>                 frag;
    Slang::ComPtr<IEntryPoint>                 compute;

    SlangResult result;
    result = module->findAndCheckEntryPoint("vert", SLANG_STAGE_VERTEX, vert.writeRef(), diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    result = module->findAndCheckEntryPoint("frag", SLANG_STAGE_FRAGMENT, vert.writeRef(), diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    result = module->findAndCheckEntryPoint("compute", SLANG_STAGE_COMPUTE, vert.writeRef(), diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    if (compute)
    {
        if (vert || frag)
        {
            LOGGER.Error(logcat::Resource, "Shader Load Error: Compute Shader不支持同时和顶点/片元Shader共存");
            return;
        }
        linking_programs.emplace_back(compute.get());
    }
    else
    {
        if (!vert || !frag)
        {
            LOGGER.Error(logcat::Resource, "Shader Load Error: 顶点/片元Shader必须同时存在");
            return;
        }
        linking_programs.emplace_back(vert.get());
        linking_programs.emplace_back(frag.get());
    }

    const core::Array<IComponentType*> components =
        linking_programs | ranges::views::transform([](const Slang::ComPtr<IComponentType>& component) { return component.get(); }) |
        ranges::to<core::Array<IComponentType*>>();

    Slang::ComPtr<slang::IComponentType> composed;
    result = session->createCompositeComponentType(components.data(), linking_programs.size(), composed.writeRef(), diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    Slang::ComPtr<IComponentType> program;
    result = composed->link(program.writeRef(), diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    // 获取各个阶段的index
    core::HashMap<ShaderStage, int> stage_index;
    module->getDefinedEntryPointCount();
    for (int i = 0; i < module->getDefinedEntryPointCount(); i++)
    {
        Slang::ComPtr<IEntryPoint> entry_point;
        VERIFY_SLANG_RESULT(module->getDefinedEntryPoint(i, entry_point.writeRef()));
        if (strcmp(entry_point->getFunctionReflection()->getName(), "vert") == 0)
        {
            stage_index[ShaderStage::Vertex] = i;
        }
        else if (strcmp(entry_point->getFunctionReflection()->getName(), "frag") == 0)
        {
            stage_index[ShaderStage::Fragment] = i;
        }
        else if (strcmp(entry_point->getFunctionReflection()->getName(), "compute") == 0)
        {
            stage_index[ShaderStage::Compute] = i;
        }
    }
    shader.stage_to_entry_point_index_ = stage_index;
    shader.linked_program_             = program;
}

void SlangShaderLoader::DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob>& diag)
{
    if (diag)
    {
        LOGGER.Error(logcat::Resource, "Shader Load Error: {}", static_cast<const char*>(diag->getBufferPointer()));
    }
}