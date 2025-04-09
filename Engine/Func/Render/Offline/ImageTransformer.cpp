//
// Created by Echo on 2025/4/9.
//

#include "ImageTransformer.hpp"

#include "Core/Math/Math.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Func/Render/Helper.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/Image.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "Resource/Assets/Texture/Texture2DMeta.hpp"

using namespace rhi;

Texture2D *ImageTransformer::TransformSphereMapToCubeMap(Texture2D *tex, Float size, rhi::Format cube_format) { return nullptr; }
