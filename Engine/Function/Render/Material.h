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

#include <glm/fwd.hpp>

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
class Material : public Object, public IDetailGUIDrawer
{
public:
    // 参数名称白名单 如果参数名字在参数白名单中，那么就不会加入到这个Material中
    // 例如MVP矩阵 每帧设置 自然不用加入
    static inline TArray<AnsiString> parameter_name_white_list = {"ubo_instance", "ubo_view"};

    // TODO: 这里应该是传入两个Shader而不是两个路径
    Material(const Path& vert, const Path& frag, const String& name);

    ~Material() override;

    void SetTexture(const AnsiString& name, Resource::Texture* texture);

    void SetTexture(const AnsiString&name, const Path& path);

    void Use(vk::CommandBuffer cb, uint32_t width = 0, uint32_t height = 0, int x = 0, int y = 0) const;

    void SetPostionViewProjection(Comp::Camera* camera);

    void SetModel(glm::mat4* models, size_t size);

    void SetPointLights(void* data, size_t size);

    void DrawMesh(vk::CommandBuffer cb, const Comp::Mesh& mesh, const TArray<uint32_t>& dynamic_offsets);

protected:
    /**
     * 解析Shader的参数
     */
    void ParseShaderParameters();

public:
    void OnInspectorGUI() override;

private:
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

    static Material* GetMaterials(const String& name);

    static Material* CreateMaterials(const Path& vert, const Path& frag, const String& name);

private:
    THashMap<String, Material*> materials_;
};

FUNCTION_NAMESPACE_END
