//
// Created by Echo on 24-12-25.
//

#include "SlangShaderLoader.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Platform/FileSystem/Path.h"
#include "Resource/Config/ResourceConfig.h"
#include "Resource/Logcat.h"
#include "Shader.h"
#include "slang.h"

#include <fstream>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

using namespace resource;
using namespace slang;
using namespace platform::rhi;
using namespace core;
using namespace platform;

#define VERIFY_SLANG_RESULT(result)                                 \
    {                                                               \
        SlangResult res = result;                                   \
        if (SLANG_FAILED(res))                                      \
        {                                                           \
            LOGGER.Error(logcat::Resource, "Slang Error: {}", res); \
            return;                                                 \
        }                                                           \
    }

static void ParseAnnotations(const StringView path, StaticArray<int, GetEnumValue(ShaderAnnotation::Count)>& output, String& name)
{
    PROFILE_SCOPE_AUTO;
    std::ifstream input(*path);
    if (!input.is_open())
    {
        return;
    }
    std::string line;
    name = Path::GetFileNameWithoutExt(path);
    while (std::getline(input, line))
    {
        if (line.empty()) continue;
        StringView line_view(line);
        StringView trimmed_line_view = line_view.Trim();
        if (trimmed_line_view.StartsWith("/**"))
        {
            continue;
        }
        if (trimmed_line_view.EndsWith("*/"))
        {
            return;
        }
        if (trimmed_line_view.StartsWith("*"))
        {
            StringView content_line = trimmed_line_view.SubString(1).TrimLeft();
            if (content_line.StartsWith("@"))
            {
                StringView annotation = content_line.SubString(1).TrimLeft();
                auto       key_value  = annotation.Split(":");
                if (key_value.size() != 2)
                {
                    LOGGER.Error(logcat::Resource, "Shader Load Error: 无效的注解: {}", annotation);
                    continue;
                }
                auto key   = key_value[0].Trim();
                auto value = key_value[1].Trim();
                if (key == "Name")
                {
                    name = value;
                }
                {
                    // 一些默认值
                    output[GetEnumValue(ShaderAnnotation::InputLayout)] = 1;
                }
                if (key == "Pipeline")
                {
                    if (value == "Graphics")
                    {
                        output[GetEnumValue(ShaderAnnotation::Pipeline)] = 1;
                    }
                    else if (value == "Compute")
                    {
                        output[GetEnumValue(ShaderAnnotation::Pipeline)] = 2;
                    }
                    else
                    {
                        LOGGER.Error(logcat::Resource, "Shader Load Error: 无效的Pipeline:注解 {}", value);
                    }
                }
                if (key == "InputLayout")
                {
                    if (value == "Vertex1")
                    {
                        output[GetEnumValue(ShaderAnnotation::InputLayout)] = 1;
                    }
                }
            }
        }
    }
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
    auto*              res_cfg      = core::GetConfig<ResourceConfig>();
    const auto&        search_paths = res_cfg->GetShaderSearchPath();
    Array<const char*> search_path_cstrs =
        search_paths | ranges::views::transform([](const core::String& path) { return *path; }) | ranges::to<core::Array<const char*>>();
    search_path_cstrs.push_back("");
    session_desc.searchPaths     = search_path_cstrs.data();
    session_desc.searchPathCount = static_cast<SlangInt>(search_path_cstrs.size());

    StaticArray<int, GetEnumValue(ShaderAnnotation::Count)> annotations{};
    ParseAnnotations(path, annotations, shader.name_);

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

    int pipeline = annotations[GetEnumValue(ShaderAnnotation::Pipeline)];
    if (pipeline == 0)
    {
        LOGGER.Error(logcat::Resource, "Shader Load Error: Shader必须指定Pipeline");
        return;
    }

    SlangResult                                result;
    core::Array<Slang::ComPtr<IComponentType>> linking_programs;
    if (pipeline == 1)   // Graphics
    {
        Slang::ComPtr<IEntryPoint> vert;
        Slang::ComPtr<IEntryPoint> frag;

        result = module->findAndCheckEntryPoint("vert", SLANG_STAGE_VERTEX, vert.writeRef(), diagnostics.writeRef());
        DiagnosticsIfNeeded(diagnostics);
        VERIFY_SLANG_RESULT(result);

        result = module->findAndCheckEntryPoint("frag", SLANG_STAGE_FRAGMENT, frag.writeRef(), diagnostics.writeRef());
        DiagnosticsIfNeeded(diagnostics);
        VERIFY_SLANG_RESULT(result);

        linking_programs.emplace_back(vert.get());
        linking_programs.emplace_back(frag.get());
    }
    else if (pipeline == 2)   // Compute
    {
        Slang::ComPtr<IEntryPoint> compute;

        result = module->findAndCheckEntryPoint("compute", SLANG_STAGE_COMPUTE, compute.writeRef(), diagnostics.writeRef());
        DiagnosticsIfNeeded(diagnostics);
        VERIFY_SLANG_RESULT(result);
        linking_programs.emplace_back(compute.get());
    }
    else
    {
        LOGGER.Error(logcat::Resource, "Shader Load Error: 无效的Pipeline: {} \n Pipeline=1指Graphics, Pipeline=2指Compute", pipeline);
        return;
    }

    const Array<IComponentType*> components =
        linking_programs | ranges::views::transform([](const Slang::ComPtr<IComponentType>& component) { return component.get(); }) |
        ranges::to<core::Array<IComponentType*>>();

    Slang::ComPtr<IComponentType> composed;
    result = session->createCompositeComponentType(
        components.data(), static_cast<SlangInt>(linking_programs.size()), composed.writeRef(), diagnostics.writeRef()
    );
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    Slang::ComPtr<IComponentType> program;
    result = composed->link(program.writeRef(), diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    // 获取各个阶段的index
    StaticArray<int, GetEnumValue(ShaderStageBits::Count)> stage_index{};
    stage_index.fill(-1);
    module->getDefinedEntryPointCount();
    for (int i = 0; i < module->getDefinedEntryPointCount(); i++)
    {
        Slang::ComPtr<IEntryPoint> entry_point;
        VERIFY_SLANG_RESULT(module->getDefinedEntryPoint(i, entry_point.writeRef()));
        if (strcmp(entry_point->getFunctionReflection()->getName(), "vert") == 0)
        {
            stage_index[GetEnumValue(ShaderStageBits::Vertex)] = i;
        }
        else if (strcmp(entry_point->getFunctionReflection()->getName(), "frag") == 0)
        {
            stage_index[GetEnumValue(ShaderStageBits::Fragment)] = i;
        }
        else if (strcmp(entry_point->getFunctionReflection()->getName(), "compute") == 0)
        {
            stage_index[GetEnumValue(ShaderStageBits::Compute)] = i;
        }
    }
    shader.stage_to_entry_point_index_ = stage_index;
    shader.linked_program_             = program;
    shader.annotations_                = annotations;
    shader.slang_session_              = session;
}

void SlangShaderLoader::DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob>& diag)
{
    if (diag)
    {
        LOGGER.Error(logcat::Resource, "Shader Load Error: {}", static_cast<const char*>(diag->getBufferPointer()));
    }
}