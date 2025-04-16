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
using namespace RHI;

#define VERIFY_SLANG_RESULT(result)                                                                                                                  \
    {                                                                                                                                                \
        SlangResult res = result;                                                                                                                    \
        if (SLANG_FAILED(res)) {                                                                                                                     \
            Log(Error) << String::Format("Slang Error: {}", res);                                                                                    \
            return;                                                                                                                                  \
        }                                                                                                                                            \
    }

void ParseAnnotations(const StringView path, Int32 (&output)[static_cast<Int32>(ShaderAnnotation::Count)], String &name)
{
    ProfileScope _(__func__);
    // 一些默认值
    output[static_cast<Int32>(ShaderAnnotation::InputLayout)] = 1;
    output[static_cast<Int32>(ShaderAnnotation::EnableDepth)] = 1;
    output[static_cast<Int32>(ShaderAnnotation::Blend)] = 0;
    output[static_cast<Int32>(ShaderAnnotation::CullMode)] = 0; // 0是Back
    output[static_cast<Int32>(ShaderAnnotation::HDR)] = 1; // 0是Back

    std::ifstream input(*path);
    if (!input.is_open())
    {
        return;
    }
    std::string line;
    name = Path::GetFileNameWithoutExt(path);
    while (std::getline(input, line))
    {
        if (line.empty())
            continue;
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
                auto key_value = annotation.Split(":");
                if (key_value.Count() != 2)
                {
                    Log(Error) << String::Format("Shader Load Error: 无效的注解: {}", annotation);
                    continue;
                }
                auto key = key_value[0].Trim();
                auto value = key_value[1].Trim();
                if (key == "Name")
                {
                    name = value;
                }

                if (key == "Pipeline")
                {
                    if (value == "Graphics")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::Pipeline)] = 1;
                    }
                    else if (value == "Compute")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::Pipeline)] = 2;
                    }
                    else
                    {
                        Log(Error) << String::Format("Shader Load Error: 无效的Pipeline:注解 {}", value);
                    }
                }
                if (key == "InputLayout")
                {
                    if (value == "Vertex1")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::InputLayout)] = 1;
                    }
                    if (value == "Vertex_UI")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::InputLayout)] = 2;
                    }
                }
                if (key == "EnableDepth")
                {
                    if (value == "false")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::EnableDepth)] = 0;
                    }
                }
                if (key == "Blend")
                {
                    if (value == "On")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::Blend)] = 1;
                    }
                }
                if (key == "CullMode")
                {
                    if (value == "Front")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::CullMode)] = 1;
                    }
                }
                if (key == "HDR")
                {
                    if (value == "Off")
                    {
                        output[static_cast<Int32>(ShaderAnnotation::HDR)] = 0;
                    }
                }
            }
        }
    }
}

void SlangShaderLoader::Load(StringView InPath, Shader &InShader)
{
    AutoLock lock(mutex_);
    ProfileScope profile("Load Shader");
    SessionDesc session_desc{};
    if (!global_session_)
    {
        createGlobalSession(global_session_.writeRef());
    }
    auto *res_cfg = GetConfig<ResourceConfig>();
    Array search_paths = res_cfg->GetShaderSearchPath();
    for (auto &search_path : search_paths)
    {
        search_path = Path::GetAbsolutePath(search_path);
    }

    Array<const char *> search_path_cstrs =
        search_paths | range::view::Select([](const String &path) {
            return *path;
        }) | range::To<Array<const char *> >();
    search_path_cstrs.Add("");
    session_desc.searchPaths = search_path_cstrs.Data();
    session_desc.searchPathCount = static_cast<SlangInt>(search_path_cstrs.Count());

    Int32 annotations[static_cast<Int32>(ShaderAnnotation::Count)];
    ParseAnnotations(InPath, annotations, InShader.name_);

    TargetDesc target_desc[2]{};
    target_desc[0].format = SLANG_SPIRV;
    target_desc[0].profile = global_session_->findProfile("glsl_460");

    target_desc[1].format = SLANG_GLSL;
    target_desc[1].profile = global_session_->findProfile("glsl_460");

    CompilerOptionEntry option_entry[3];
    if (RHI::GetGfxContextRef().GetAPI() == RHI::GraphicsAPI::Vulkan)
    {
        option_entry[0].name = CompilerOptionName::EmitSpirvDirectly;
        option_entry[0].value.intValue0 = 1;
        option_entry[1].name = CompilerOptionName::DebugInformation;
        option_entry[1].value.intValue0 = SLANG_DEBUG_INFO_LEVEL_MAXIMAL;
        option_entry[2].name = CompilerOptionName::DebugInformationFormat;
        option_entry[2].value.intValue0 = SLANG_DEBUG_INFO_FORMAT_PDB;
        target_desc[0].compilerOptionEntryCount = 3;
        target_desc[0].compilerOptionEntries = option_entry;
    }

    session_desc.targets = target_desc;
    session_desc.targetCount = 2;

    Slang::ComPtr<ISession> session;
    global_session_->createSession(session_desc, session.writeRef());

    Slang::ComPtr<IBlob> Diagnostics;
    Slang::ComPtr<IModule> Module;
    Module = session->loadModule(*InPath, Diagnostics.writeRef());
    DiagnosticsIfNeeded(Diagnostics);
    if (!Module)
        return;

    int Pipeline = annotations[static_cast<Int32>(ShaderAnnotation::Pipeline)];
    if (Pipeline == 0)
    {
        Log(Error) << "Shader Load Error: Shader必须指定Pipeline";
        return;
    }

    SlangResult Result;
    Array<Slang::ComPtr<IComponentType> > linking_programs;
    Array<Format> output_formats;
    Slang::ComPtr<IEntryPoint> vert;
    Slang::ComPtr<IEntryPoint> frag;
    Slang::ComPtr<IEntryPoint> compute;
    if (Pipeline == 1) // Graphics
    {
        Result = Module->findAndCheckEntryPoint("Vertex", SLANG_STAGE_VERTEX, vert.writeRef(), Diagnostics.writeRef());
        DiagnosticsIfNeeded(Diagnostics);
        VERIFY_SLANG_RESULT(Result);

        Result = Module->findAndCheckEntryPoint("Fragment", SLANG_STAGE_FRAGMENT, frag.writeRef(), Diagnostics.writeRef());
        DiagnosticsIfNeeded(Diagnostics);
        VERIFY_SLANG_RESULT(Result);

        IComponentType *Temp = vert.get();
        Slang::ComPtr<IComponentType> Temp1;
        *Temp1.writeRef() = Temp;
        linking_programs.Add(Temp1);
        Temp = frag.get();
        *Temp1.writeRef() = Temp;
        linking_programs.Add(Temp1);
    }
    else if (Pipeline == 2) // Compute
    {
        Result = Module->findAndCheckEntryPoint("Compute", SLANG_STAGE_COMPUTE, compute.writeRef(), Diagnostics.writeRef());
        DiagnosticsIfNeeded(Diagnostics);
        VERIFY_SLANG_RESULT(Result);
        IComponentType *temp = compute.get();
        Slang::ComPtr<IComponentType> temp1;
        *temp1.writeRef() = temp;
        linking_programs.Add(temp1);
    }
    else
    {
        Log(Error) << String::Format("Shader Load Error: 无效的Pipeline: {} \n Pipeline=1指Graphics, Pipeline=2指Compute", Pipeline);
        return;
    }

    const Array<IComponentType *> components = linking_programs |
                                               range::view::Select([](const Slang::ComPtr<IComponentType> &component) {
                                                   return component.get();
                                               }) |
                                               range::To<Array<IComponentType *> >();

    Slang::ComPtr<IComponentType> composed;
    Result = session->createCompositeComponentType(components.Data(), static_cast<SlangInt>(linking_programs.Count()), composed.writeRef(),
                                                   Diagnostics.writeRef());
    DiagnosticsIfNeeded(Diagnostics);
    VERIFY_SLANG_RESULT(Result);

    Slang::ComPtr<IComponentType> program;
    Result = composed->link(program.writeRef(), Diagnostics.writeRef());
    DiagnosticsIfNeeded(Diagnostics);
    VERIFY_SLANG_RESULT(Result);

    // 获取各个阶段的index
    Int32 StageIndex[Shader::SHADER_STAGE_COUNT];
    range::Fill(StageIndex, -1);
    Module->getDefinedEntryPointCount();
    for (int i = 0; i < Module->getDefinedEntryPointCount(); i++)
    {
        Slang::ComPtr<IEntryPoint> entry_point;
        VERIFY_SLANG_RESULT(Module->getDefinedEntryPoint(i, entry_point.writeRef()));
        if (strcmp(entry_point->getFunctionReflection()->getName(), "Vertex") == 0)
        {
            StageIndex[Shader::VERTEX_STAGE_IDX] = i;
        }
        else if (strcmp(entry_point->getFunctionReflection()->getName(), "Fragment") == 0)
        {
            StageIndex[Shader::FRAGMENT_STAGE_IDX] = i;
        }
        else if (strcmp(entry_point->getFunctionReflection()->getName(), "Compute") == 0)
        {
            StageIndex[Shader::COMPUTE_STAGE_IDX] = i;
        }
    }
    CopyArray(StageIndex, InShader.mStageToEntryPointIndex);
    InShader.mLinedProgram = program;
    CopyArray(annotations, InShader.mAnnotations);
    InShader.mSlangSession = session;
}

void SlangShaderLoader::DiagnosticsIfNeeded(const Slang::ComPtr<slang::IBlob> &diag)
{
    if (diag)
    {
        Log(Error) << "Shader Load Error "_es + static_cast<const char *>(diag->getBufferPointer());
    }
}