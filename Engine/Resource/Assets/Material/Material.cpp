//
// Created by Echo on 25-1-16.
//

#include "Material.h"

#include "Core/Serialization/YamlArchive.h"
#include "MaterialMeta.h"
#include "Platform/FileSystem/Path.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"

#include GEN_HEADER("Resource.Material.generated.h")
GENERATED_SOURCE()

using namespace resource;
using namespace core;
using namespace platform;
using namespace rhi;

void Material::PerformLoad()
{
    YamlArchive archive;
    auto        op_meta = AssetDataBase::QueryMeta<MaterialMeta>(GetHandle());
    if (!op_meta)
    {
        LOGGER.Error(logcat::Resource, "加载失败: handle为{}的Shader不在资产数据库", GetHandle());
        return;
    }
    auto&      meta     = *op_meta;
    StringView mat_path = meta.path;
    if (!Path::IsExist(mat_path))
    {
        LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Shader文件不存在", mat_path);
        return;
    }
    if (!mat_path.EndsWith(".mat"))
    {
        LOGGER.Error(logcat::Resource, "加载失败: Shader必须以.mat: {}", mat_path);
        return;
    }
    auto op_content = File::ReadAllText(mat_path);
    if (!op_content)
    {
        LOGGER.Error(logcat::Resource, "加载失败: 无法读取路径为{}的Shader文件", mat_path);
        return;
    }
    auto& content = *op_content;
    if (!archive.Deserialize(*content, this, GetType()))
    {
        LOGGER.Error(logcat::Resource, "加载失败: 无法反序列化路径为{}的Shader文件", mat_path);
        return;
    }
    Shader* shader_ptr = (Shader*)shader_;
    if (shader_ptr == nullptr)
    {
        LOGGER.Error(logcat::Resource, "加载失败: 材质的Shader为空");
        return;
    }
    GraphicsPipelineDesc desc{};
    if (!shader_ptr->FillGraphicsPSODescFromShader(desc))
    {
        LOGGER.Error(logcat::Resource, "加载失败: 无法填充GraphicsPipelineDesc");
        return;
    }
    desc.attachments.depth_format = GetGfxContextRef().GetDefaultDepthStencilFormat();
    desc.attachments.color_formats.push_back(GetGfxContextRef().GetDefaultColorFormat());
    active_pipeline_ = GetGfxContextRef().CreateGraphicsPipeline(desc, nullptr);
    descriptor_set_  = AllocateDescriptorSetFunc(desc.descriptor_set_layouts[0]);
    // TODO: Update DescriptorSet
}

void Material::PerformUnload()
{
}