/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */
#include "Shader.h"

#include "spirv_cross.hpp"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "Utils/StringUtils.h"

#include "shaderc/shaderc.hpp"
#include "Utils/HashUtils.h"

#include "nlohmann/json.hpp"


#include <utility>

namespace rhi::vulkan
{
vk::DescriptorType GetVkDescriptorType(const EUniformDescriptorType type)
{
    switch (type)
    {
    case EUniformDescriptorType::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
    case EUniformDescriptorType::Sampler2D: return vk::DescriptorType::eCombinedImageSampler;
    case EUniformDescriptorType::DynamicUniformBuffer: return vk::DescriptorType::eUniformBufferDynamic;
    case EUniformDescriptorType::SamplerCube: return vk::DescriptorType::eCombinedImageSampler;
    }
    return vk::DescriptorType::eUniformBuffer;
}

vk::ShaderStageFlagBits GetVkShaderStage(const EShaderStage stage)
{
    switch (stage)
    {
    case EShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex;
    case EShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
    default: return vk::ShaderStageFlagBits::eVertex;
    }
}

String ShaderStageToString(const EShaderStage stage)
{
    switch (stage)
    {
    case EShaderStage::Vertex: return L"顶点shader";
    case EShaderStage::Fragment: return L"片元shader";
    case EShaderStage::None: return L"";
    }
    return L"";
}

class ShaderCache
{
public:
    void Init()
    {
        if (init_)
        {
            return;
        }
        Path cache_path = L"Cache/Shaders";
        if (!cache_path.IsExist())
        {
            cache_path.CreateDir();
        }
        Path cache_json_path = L"Cache/Shaders/shader_cache.json";
        if (cache_json_path.IsExist())
        {
            auto json = cache_json_path.ReadAllText();
            try
            {
                json_ = nlohmann::json::parse(json.data());
            }
            catch (const std::exception& e)
            {
                LOG_ERROR_ANSI_CATEGORY(Shader, "Parse shader cache file failed: {}", e.what());
                return;
            }
        }
        else
        {
            cache_json_path.CreateFileA();
        }
        init_ = true;
    }

#undef GetObject
    void RequestShaderSpirv(const Path& path, EShaderStage stage, Array<uint32_t>& code)
    {
        auto spirv_path      = Path(String(L"Cache/Shaders/") + path.GetFileName() + L".spv");
        auto key             = path.ToRelativeAnsiString();
        auto new_source_hash = HashUtils::ComputeSHA256(path);
        if (json_.contains(key.c_str()))
        {
            auto&      cache = json_[key.c_str()];
            AnsiString old_source_hash;
            AnsiString old_spirv_hash;
            if (cache.contains("source_hash"))
            {
                old_source_hash = cache["source_hash"].get<AnsiString>();
            }
            if (cache.contains("spirv_hash"))
            {
                old_spirv_hash = cache["spirv_hash"].get<AnsiString>();
            }
            if (spirv_path.IsExist())
            {
                auto new_spirv_hash = HashUtils::ComputeSHA256(spirv_path);
                if (new_spirv_hash.has_value() && new_source_hash.has_value())
                {
                    if (old_source_hash == new_source_hash.value() && old_spirv_hash == new_spirv_hash.value())
                    {
                        return spirv_path.ReadAllBinary(code);
                    }
                }
            }
        }
        if (Shader::CompileShaderCode2Spirv(path.GetFileName(), path.ReadAllText(), stage, code))
        {
            spirv_path.WriteAllBinary(code);
            auto new_spirv_hash               = HashUtils::ComputeSHA256(spirv_path);
            json_[key.c_str()]["source_hash"] = new_source_hash.value();
            json_[key.c_str()]["spirv_hash"]  = new_spirv_hash.value();
            Path cache_json_path              = L"Cache/Shaders/shader_cache.json";
            cache_json_path.WriteAllText(json_.dump());
        }
    }

protected:
    nlohmann::json json_;
    bool           init_ = false;
};

void ShaderManager::DestroyAll()
{
    for (auto& val: shaders_ | std::views::values)
    {
        Delete(val);
    }
    shaders_.clear();
}

Shader::Shader(Protected, const Ref<LogicalDevice> device, const Path& shader_path, EShaderStage shader_stage, const AnsiString& shader_name) :
    shader_stage_(shader_stage), shader_path_(shader_path), device_(device)
{
    uint32_t*        shader_code_ptr  = nullptr;
    size_t           shader_code_size = 0;
    Array<uint32_t> shader_code;
    if (!cache_)
    {
        cache_ = MakeShared<ShaderCache>();
        cache_->Init();
    }
    if (shader_path.EndsWith(L".spv"))
    {
        LOG_ERROR_CATEGORY(Shader, L"{}: 不支持以spv为结尾的shader文件,请使用以.vert或.frag为结尾的文本文件", shader_path.ToAbsoluteString());
    }
    else
    {
        cache_->RequestShaderSpirv(shader_path, shader_stage, shader_code);
    }
    shader_code_ptr  = shader_code.data();
    shader_code_size = shader_code.size();
    // 解析此Shader的Input
    ParseShaderCode(shader_code_ptr, shader_code_size, shader_stage);
    // 创建ShaderModule
    vk::ShaderModuleCreateInfo create_info = {};
    create_info.setCodeSize(shader_code_size * 4).setPCode(shader_code_ptr);
    shader_module_ = device.get().GetHandle().createShaderModule(create_info);

    shader_name_ = shader_name;
    if (!shader_name_.empty())
    {
        device.get().SetShaderModuleDebugName(shader_module_, shader_name_.data());
    }
}

static AnsiString PreprocessHeader(const AnsiString& source_name, EShaderStage stage, const AnsiString& source)
{
    shaderc::Compiler       compiler;
    shaderc::CompileOptions options;
    shaderc_shader_kind     kind = shaderc_miss_shader;
    switch (stage)
    {
    case EShaderStage::Vertex: kind = shaderc_glsl_vertex_shader; break;
    case EShaderStage::Fragment: kind = shaderc_glsl_fragment_shader; break;
    case EShaderStage::None: break;
    }
    shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(source, kind, source.c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        LOG_ERROR_ANSI_CATEGORY(Shader, "compile shader {} failed: {}", source_name, result.GetErrorMessage());
        return "";
    }
    return {result.cbegin(), result.cend()};
}

bool CompileFile(const AnsiString& name, EShaderStage stage, const AnsiString& source, Array<uint32_t>& output, bool optimize = false)
{
    shaderc::Compiler       compiler;
    shaderc::CompileOptions options;
    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_size);
    }
    shaderc_shader_kind kind = shaderc_miss_shader;
    switch (stage)
    {
    case EShaderStage::Vertex: kind = shaderc_glsl_vertex_shader; break;
    case EShaderStage::Fragment: kind = shaderc_glsl_fragment_shader; break;
    case EShaderStage::None: break;
    }
    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, kind, name.c_str(), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        LOG_ERROR_ANSI_CATEGORY_NO_BREAK(Shader, "compile shader {} failed: {}", name, module.GetErrorMessage());
        output.clear();
        return false;
    }

    output = {module.cbegin(), module.cend()};
    return true;
}

// TODO: 添加条件编译选项
bool Shader::CompileShaderCode2Spirv(const String& shader_name, const AnsiString& shader_source, EShaderStage shader_stage, Array<uint32_t>& output)
{
    LOG_INFO_CATEGORY(Shader, L"正在编译 {} {}...", ShaderStageToString(shader_stage), shader_name);
    const AnsiString shader_ansi_string = StringUtils::ToAnsiString(shader_name);
    const auto       preprocessed       = PreprocessHeader(shader_ansi_string, shader_stage, shader_source);
    bool             success            = CompileFile(shader_ansi_string, shader_stage, preprocessed, output);
    if (success)
    {
        LOG_INFO_CATEGORY(Shader, L"{} {} 编译完成.", ShaderStageToString(shader_stage), shader_name);
        return true;
    }
    else
    {
        LOG_ERROR_CATEGORY_NO_BREAK(Shader, L"{} {} 编译失败.", ShaderStageToString(shader_stage), shader_name);
        return false;
    }
}


void Shader::ParseShaderCode(const uint32_t* shader_code, size_t shader_code_size, EShaderStage shader_stage)
{
    shader_stage_ = shader_stage;
    using namespace spirv_cross;
    Compiler        compiler(shader_code, shader_code_size);
    ShaderResources res = compiler.get_shader_resources();
    // 收集所有layout(location = 0) in vec3 inPos;信息
    {
        // 只有顶点输入需要在管线建立
        if (shader_stage_ != EShaderStage::Vertex)
        {
            return;
        }

        for (const auto& In: res.stage_inputs)
        {
            VertexInAttribute in_attr;
            in_attr.name     = compiler.get_name(In.id);
            in_attr.location = compiler.get_decoration(In.id, spv::DecorationLocation);
            auto t           = compiler.get_type(In.type_id);
            in_attr.width    = t.width / 8;
            in_attr.size     = in_attr.width * t.vecsize;
            in_attributes_.push_back(in_attr);
        }
        std::ranges::sort(in_attributes_, [](const VertexInAttribute& lhs, const VertexInAttribute& rhs) { return lhs.location < rhs.location; });
        // 计算偏移
        size_t offset = 0;
        for (auto& InAttr: in_attributes_)
        {
            InAttr.offset = offset;
            offset += InAttr.size;
        }
    }
}

Shader::~Shader()
{
    const auto& device = device_.get();
    device.DestroyShaderModule(shader_module_);
}
}
