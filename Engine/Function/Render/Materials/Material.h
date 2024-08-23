/**
 * @file Material.h
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#pragma once
#include "Component/Interface/IDetailGUIDrawer.h"
#include "CoreDef.h"
#include "FunctionCommon.h"
#include "Object/Object.h"
#include "ResourceManager.h"
#include "RHI/Vulkan/Render/Shader.h"

#include <glm/fwd.hpp>

namespace RHI::Vulkan
{
class RenderPass;
}
namespace Function::Comp
{
class Mesh;
}
namespace Function::Comp
{
class Camera;
}
namespace RHI::Vulkan
{
class GraphicsPipeline;
}
namespace RHI::Vulkan
{
class ShaderProgram;
}
namespace RHI::Vulkan
{
class Shader;
}
FUNCTION_NAMESPACE_BEGIN

/**
 * 材质的封装
 * 作用：Shader参数的集合
 *      通过此类设置Shader参数
 */

enum class EDepthCompareOp
{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,
};

struct MaterialConfig
{
    bool            use_counter_clock_wise_front_face = true;   // 正面是不是逆时针
    bool            use_depth_test                    = true;   // 是否使用深度测试
    bool            use_depth_write                   = true;   // 是否使用深度写入
    EDepthCompareOp depth_compare_op                  = EDepthCompareOp::LessOrEqual;
};

class Material : public Object, public IDetailGUIDrawer
{
public:
    // 参数名称白名单 如果参数名字在参数白名单中，那么就不会加入到这个Material中
    // 例如MVP矩阵 每帧设置 自然不用加入
    static inline TArray<AnsiString> parameter_name_white_list = {"ubo_instance", "ubo_view"};

    // TODO: 这里应该是传入两个Shader而不是两个路径
    Material(const Path& vert, const Path& frag, const MaterialConfig& config = {}, const String& name = L"");

    Material(
        RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, const Type& pass_type, const MaterialConfig& config = {}, const String& name = L""
    );

    Material(
        RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, RHI::Vulkan::RenderPass* render_pass, const MaterialConfig& config = {},
        const String& name = L""
    );

    ~Material() override;

    // SetTexture的不同重载
    void SetTexture(const AnsiString& name, Resource::Texture* texture);
    void SetTexture(const AnsiString& name, const Path& path);
    void SetTexture(const AnsiString& name, const RHI::Vulkan::ImageView& view, const RHI::Vulkan::Sampler& sampler);

    void SetCubeTexture(const AnsiString& name, const RHI::Vulkan::ImageView& view, const RHI::Vulkan::Sampler& sampler);

    void Use(vk::CommandBuffer cb, uint32_t width = 0, uint32_t height = 0, int x = 0, int y = 0) const;

    void SetPostionViewProjection(Comp::Camera* camera);

    void SetModel(glm::mat4* models, size_t size);

    void Set(const AnsiString& name, void* data, size_t size);

    void SetPointLights(void* data, size_t size);

    void DrawMesh(vk::CommandBuffer cb, const Comp::Mesh& mesh, const TArray<uint32_t>& dynamic_offsets);

    void PushConstant(vk::CommandBuffer cb, uint32_t offset, uint32_t size, RHI::Vulkan::EShaderStage stage, void* data) const;

    /**
     * 判断以name为名字的纹理参数是否已经被设置
     * @param name
     * @return
     */
    bool HasTextureSet(const AnsiString& name);

    /**
     * 获得name对应的Texture
     * @param name
     * @return
     */
    Resource::Texture* GetTextureValue(const AnsiString& name);

protected:
    /**
     * 解析Shader的参数
     */
    void ParseShaderParameters();

public:
    void OnInspectorGUI() override;

protected:
    RHI::Vulkan::ShaderProgram*    shader_program_ = nullptr;
    RHI::Vulkan::GraphicsPipeline* pipeline_       = nullptr;

    // 存储shader里所有的纹理参数
    THashMap<AnsiString, Resource::Texture*> textures_maps_;

    bool texture_map_dirty_ = false;
};

class MaterialManager : public Singleton<MaterialManager>
{
public:
    MaterialManager();

    static void DestroyMaterials();

    static Material* GetMaterial(const String& name);

    static Material* CreateMaterial(const Path& vert, const Path& frag, const String& name = {}, const MaterialConfig& config = {});

    static Material* CreateMaterial(
        RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, const Type& pass_type, const String& name = {}, const MaterialConfig& config = {}
    );

    static Material* CreateMaterial(
        RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, RHI::Vulkan::RenderPass* render_pass, const String& name = {},
        const MaterialConfig& config = {}

    );

    template<typename T>
        requires std::is_base_of_v<Material, T>
    static T* CreateMaterial(
        RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, RHI::Vulkan::RenderPass* render_pass, const String& name = {},
        const MaterialConfig& config = {}
    )
    {
        auto& mats = Get()->materials_;
        if (mats.contains(name))
        {
            LOG_WARNING_CATEGORY(Material, L"已存在同名材质{},进行覆盖", name);
            delete mats[name];
            mats[name] = new T(vert, frag, render_pass, config, name);
        }
        else
        {
            mats[name] = new T(vert, frag, render_pass, config, name);
        }
        return static_cast<T*>(mats[name]);

    }

private:
    THashMap<String, Material*> materials_;
};

FUNCTION_NAMESPACE_END
