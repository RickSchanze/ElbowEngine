//
// Created by Echo on 2025/3/25.
//

#include "SlangShaderLoader.hpp"

#include <fstream>

#include "Core/Config/ConfigManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Misc/Other.hpp"
#include "Core/Profile.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Resource/Config.hpp"

using namespace slang;
using namespace rhi;

#define VERIFY_SLANG_RESULT(result)                                                                                                                  \
    {                                                                                                                                                \
        SlangResult res = result;                                                                                                                    \
        if (SLANG_FAILED(res)) {                                                                                                                     \
            Log(Error) << String::Format("Slang Error: {}", res);                                                                                    \
            return;                                                                                                                                  \
        }                                                                                                                                            \
    }

void ParseAnnotations(const StringView path, Int32 (&output)[static_cast<Int32>(ShaderAnnotation::Count)], String &name) {
    ProfileScope _(__func__);
    // 一些默认值
    output[static_cast<Int32>(ShaderAnnotation::InputLayout)] = 1;
    output[static_cast<Int32>(ShaderAnnotation::EnableDepth)] = 1;
    output[static_cast<Int32>(ShaderAnnotation::Blend)] = 0;
    output[static_cast<Int32>(ShaderAnnotation::CullMode)] = 0; // 0是Back

    std::ifstream input(*path);
    if (!input.is_open()) {
        return;
    }
    std::string line;
    name = Path::GetFileNameWithoutExt(path);
    while (std::getline(input, line)) {
        if (line.empty())
            continue;
        StringView line_view(line);
        StringView trimmed_line_view = line_view.Trim();
        if (trimmed_line_view.StartsWith("/**")) {
            continue;
        }
        if (trimmed_line_view.EndsWith("*/")) {
            return;
        }
        if (trimmed_line_view.StartsWith("*")) {
            StringView content_line = trimmed_line_view.SubString(1).TrimLeft();
            if (content_line.StartsWith("@")) {
                StringView annotation = content_line.SubString(1).TrimLeft();
                auto key_value = annotation.Split(":");
                if (key_value.Count() != 2) {
                    Log(Error) << String::Format("Shader Load Error: 无效的注解: {}", annotation);
                    continue;
                }
                auto key = key_value[0].Trim();
                auto value = key_value[1].Trim();
                if (key == "Name") {
                    name = value;
                }

                if (key == "Pipeline") {
                    if (value == "Graphics") {
                        output[static_cast<Int32>(ShaderAnnotation::Pipeline)] = 1;
                    } else if (value == "Compute") {
                        output[static_cast<Int32>(ShaderAnnotation::Pipeline)] = 2;
                    } else {
                        Log(Error) << String::Format("Shader Load Error: 无效的Pipeline:注解 {}", value);
                    }
                }
                if (key == "InputLayout") {
                    if (value == "Vertex1") {
                        output[static_cast<Int32>(ShaderAnnotation::InputLayout)] = 1;
                    }
                    if (value == "Vertex_UI") {
                        output[static_cast<Int32>(ShaderAnnotation::InputLayout)] = 2;
                    }
                }
                if (key == "EnableDepth") {
                    if (value == "false") {
                        output[static_cast<Int32>(ShaderAnnotation::EnableDepth)] = 0;
                    }
                }
                if (key == "Blend") {
                    if (value == "On") {
                        output[static_cast<Int32>(ShaderAnnotation::Blend)] = 1;
                    }
                }
                if (key == "CullMode") {
                    if (value == "Front") {
                        output[static_cast<Int32>(ShaderAnnotation::CullMode)] = 1;
                    }
                }
            }
        }
    }
}

void SlangShaderLoader::Load(StringView path, Shader &shader) {
    AutoLock lock(mutex_);
    ProfileScope profile("Load Shader");
    SessionDesc session_desc{};
    if (!global_session_) {
        createGlobalSession(global_session_.writeRef());
    }
    auto *res_cfg = GetConfig<ResourceConfig>();
    Array search_paths = res_cfg->GetShaderSearchPath();
    for (auto &search_path: search_paths) {
        search_path = Path::GetAbsolutePath(search_path);
    }

    Array<const char *> search_path_cstrs =
            search_paths | range::view::Select([](const String &path) { return *path; }) | range::To<Array<const char *>>();
    search_path_cstrs.Add("");
    session_desc.searchPaths = search_path_cstrs.Data();
    session_desc.searchPathCount = static_cast<SlangInt>(search_path_cstrs.Count());

    Int32 annotations[static_cast<Int32>(ShaderAnnotation::Count)];
    ParseAnnotations(path, annotations, shader.name_);

    TargetDesc target_desc[2]{};
    target_desc[0].format = SLANG_SPIRV;
    target_desc[0].profile = global_session_->findProfile("glsl_460");

    target_desc[1].format = SLANG_GLSL;
    target_desc[1].profile = global_session_->findProfile("glsl_460");

    CompilerOptionEntry option_entry[2];
    if (rhi::GetGfxContextRef().GetAPI() == rhi::GraphicsAPI::Vulkan) {
        option_entry[0].name = CompilerOptionName::EmitSpirvDirectly;
        option_entry[0].value.intValue0 = 1;
        option_entry[1].name = CompilerOptionName::DebugInformation;
        option_entry[1].value.intValue0 = SLANG_DEBUG_INFO_LEVEL_STANDARD;
        target_desc[0].compilerOptionEntryCount = 2;
        target_desc[0].compilerOptionEntries = option_entry;
    }

    session_desc.targets = target_desc;
    session_desc.targetCount = 2;

    Slang::ComPtr<ISession> session;
    global_session_->createSession(session_desc, session.writeRef());

    Slang::ComPtr<IBlob> diagnostics;
    Slang::ComPtr<IModule> module;
    module = session->loadModule(*path, diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    if (!module)
        return;

    int pipeline = annotations[static_cast<Int32>(ShaderAnnotation::Pipeline)];
    if (pipeline == 0) {
        Log(Error) << "Shader Load Error: Shader必须指定Pipeline";
        return;
    }

    SlangResult result;
    Array<Slang::ComPtr<IComponentType>> linking_programs;
    Array<Format> output_formats;
    Slang::ComPtr<IEntryPoint> vert;
    Slang::ComPtr<IEntryPoint> frag;
    Slang::ComPtr<IEntryPoint> compute;
    if (pipeline == 1) // Graphics
    {
        result = module->findAndCheckEntryPoint("vert", SLANG_STAGE_VERTEX, vert.writeRef(), diagnostics.writeRef());
        DiagnosticsIfNeeded(diagnostics);
        VERIFY_SLANG_RESULT(result);

        result = module->findAndCheckEntryPoint("frag", SLANG_STAGE_FRAGMENT, frag.writeRef(), diagnostics.writeRef());
        DiagnosticsIfNeeded(diagnostics);
        VERIFY_SLANG_RESULT(result);

        IComponentType *temp = vert.get();
        Slang::ComPtr<IComponentType> temp1;
        *temp1.writeRef() = temp;
        linking_programs.Add(temp1);
        temp = frag.get();
        *temp1.writeRef() = temp;
        linking_programs.Add(temp1);
    } else if (pipeline == 2) // Compute
    {
        result = module->findAndCheckEntryPoint("compute", SLANG_STAGE_COMPUTE, compute.writeRef(), diagnostics.writeRef());
        DiagnosticsIfNeeded(diagnostics);
        VERIFY_SLANG_RESULT(result);
        IComponentType *temp = compute.get();
        Slang::ComPtr<IComponentType> temp1;
        *temp1.writeRef() = temp;
        linking_programs.Add(temp1);
    } else {
        Log(Error) << String::Format("Shader Load Error: 无效的Pipeline: {} \n Pipeline=1指Graphics, Pipeline=2指Compute", pipeline);
        return;
    }

    const Array<IComponentType *> components = linking_programs |
                                               range::view::Select([](const Slang::ComPtr<IComponentType> &component) { return component.get(); }) |
                                               range::To<Array<IComponentType *>>();

    Slang::ComPtr<IComponentType> composed;
    result = session->createCompositeComponentType(components.Data(), static_cast<SlangInt>(linking_programs.Count()), composed.writeRef(),
                                                   diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    Slang::ComPtr<IComponentType> program;
    result = composed->link(program.writeRef(), diagnostics.writeRef());
    DiagnosticsIfNeeded(diagnostics);
    VERIFY_SLANG_RESULT(result);

    // 获取各个阶段的index
    Int32 stage_index[Shader::SHADER_STAGE_COUNT];
    range::Fill(stage_index, -1);
    module->getDefinedEntryPointCount();
    for (int i = 0; i < module->getDefinedEntryPointCount(); i++) {
        Slang::ComPtr<IEntryPoint> entry_point;
        VERIFY_SLANG_RESULT(module->getDefinedEntryPoint(i, entry_point.writeRef()));
        if (strcmp(entry_point->getFunctionReflection()->getName(), "vert") == 0) {
            stage_index[Shader::VERTEX_STAGE_IDX] = i;
        } else if (strcmp(entry_point->getFunctionReflection()->getName(), "frag") == 0) {
            stage_index[Shader::FRAGMENT_STAGE_IDX] = i;
        } else if (strcmp(entry_point->getFunctionReflection()->getName(), "compute") == 0) {
            stage_index[Shader::COMPUTE_STAGE_IDX] = i;
        }
    }
    CopyArray(stage_index, shader.stage_to_entry_point_index_);
    shader.linked_program_ = program;
    CopyArray(annotations, shader.annotations_);
    shader.slang_session_ = session;
}

void SlangShaderLoader::DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob> &diag) {
    if (diag) {
        Log(Error) << "Shader Load Error "_es + static_cast<const char *>(diag->getBufferPointer());
    }
}
