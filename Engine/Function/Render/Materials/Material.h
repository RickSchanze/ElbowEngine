/**
 * @file Material.h
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#pragma once
#include "Component/Interface/IDetailGUIDrawer.h"
#include "CoreDef.h"
#include "Object/Object.h"
#include "ResourceManager.h"
#include "RHI/Vulkan/Render/Shader.h"

#include <glm/fwd.hpp>

namespace function::comp
{
class Mesh;
class Camera;
}   // namespace Function::Comp

namespace rhi::vulkan
{
class RenderPass;
class GraphicsPipeline;
class ShaderProgram;
class Shader;
}   // namespace rhi::vulkan

namespace function {

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
    bool            has_vertex_input_binding          = true;   // 是否有顶点输入绑定
    EDepthCompareOp depth_compare_op                  = EDepthCompareOp::LessOrEqual;
};

class Material : public Object, public IDetailGUIDrawer
{
public:
    // 参数名称白名单 如果参数名字在参数白名单中，那么就不会加入到这个Material中
    // 例如MVP矩阵 每帧设置 自然不用加入
    static inline Array<AnsiString> parameter_name_white_list = {"ubo_instance", "ubo_view"};

    Material(const Path& vert, const Path& frag, const MaterialConfig& config = {}, const String& name = L"");

    Material(
        rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, const Type& pass_type, const MaterialConfig& config = {}, const String& name = L""
    );

    Material(
        rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, rhi::vulkan::RenderPass* render_pass, const MaterialConfig& config = {},
        const String& name = L""
    );

    ~Material() override;

    // SetTexture的不同重载
    void      SetTexture(const AnsiString& name, resource::Texture* texture);
    Material &SetTexture(const AnsiString& name, const Path& path);
    void SetTexture(const AnsiString& name, const rhi::vulkan::ImageView& view, const rhi::vulkan::Sampler& sampler);

    void SetCubeTexture(const AnsiString& name, const rhi::vulkan::ImageView& view, const rhi::vulkan::Sampler& sampler);

    void Use(vk::CommandBuffer cb, uint32_t width = 0, uint32_t height = 0, int x = 0, int y = 0) const;

    void SetPositionViewProjection(comp::Camera* camera);

    void SetModel(glm::mat4* models, size_t size);

    void Set(const AnsiString& name, void* data, size_t size);

    void SetPointLights(void* data, size_t size);

    void DrawMesh(vk::CommandBuffer cb, const comp::Mesh& mesh, const Array<uint32_t>& dynamic_offsets);

    void Draw(vk::CommandBuffer cb, uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0, uint32_t first_instance = 0);

    void PushConstant(vk::CommandBuffer cb, uint32_t offset, uint32_t size, rhi::vulkan::EShaderStage stage, void* data) const;

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
    resource::Texture* GetTextureValue(const AnsiString& name);

protected:
    /**
     * 解析Shader的参数
     */
    void ParseShaderParameters();

public:
    void OnInspectorGUI() override;

protected:
    rhi::vulkan::ShaderProgram*    shader_program_ = nullptr;
    rhi::vulkan::GraphicsPipeline* pipeline_       = nullptr;

    // 存储shader里所有的纹理参数
    HashMap<AnsiString, resource::Texture*> textures_maps_;

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
        rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, const Type& pass_type, const String& name = {}, const MaterialConfig& config = {}
    );

    static Material* CreateMaterial(
        rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, rhi::vulkan::RenderPass* render_pass, const String& name = {},
        const MaterialConfig& config = {}
    );

    template<typename T>
        requires std::is_base_of_v<Material, T>
    static T* CreateMaterial(
        rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, rhi::vulkan::RenderPass* render_pass, const String& name = {},
        const MaterialConfig& config = {}
    )
    {
        auto& mats = Get()->materials_;
        if (mats.contains(name))
        {
            LOG_WARNING_CATEGORY(Material, L"已存在同名材质{},进行覆盖", name);
            Delete(mats[name]);
            mats[name] = New<T>(vert, frag, render_pass, config, name);
        }
        else
        {
            mats[name] = New<T>(vert, frag, render_pass, config, name);
        }
        return static_cast<T*>(mats[name]);

    }

private:
    HashMap<String, Material*> materials_;
};

}
