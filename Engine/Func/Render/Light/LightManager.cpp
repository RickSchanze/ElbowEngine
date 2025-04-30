//
// Created by Echo on 2025/4/5.
//

#include "LightManager.hpp"

#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "PointLightComponent.hpp"

using namespace RHI;


void LightManager::Startup()
{
    BufferDesc desc{sizeof(DynamicGlobalLights), BUB_UniformBuffer, BMPB_HostVisible | BMPB_HostCoherent};
    global_light_buffer_ = RHI::GetGfxContextRef().CreateBuffer(desc);
    dynamic_global_lights_ = reinterpret_cast<DynamicGlobalLights *>(global_light_buffer_->BeginWrite());
}

void LightManager::Shutdown()
{
    global_light_buffer_->EndWrite();
    global_light_buffer_ = nullptr;
    dynamic_global_lights_ = nullptr;
}

void LightManager::AddLight(PointLightComponent *point_light_component)
{
    if (point_light_component == nullptr)
    {
        VLOG_WARN("传入的点光源组件为空");
        return;
    }
    auto &self = GetByRef();
    if (self.point_light_map_.Contains(point_light_component))
    {
        return;
    }
    Int8 index = self.FindNextAvailablePointLightIndex();
    if (index != -1)
    {
        self.occupied_[index] = true;
        self.point_light_map_[point_light_component] = index;
        Int32 count = self.dynamic_global_lights_->point_light_count + 1;
        memcpy(&self.dynamic_global_lights_->point_light_count, &count, sizeof(Int32));
    }
    else
    {
        VLOG_ERROR("动态光源数量超出限制");
    }
    UpdateLight(point_light_component);
}

void LightManager::UpdateLight(PointLightComponent *point_light_component)
{
    if (point_light_component == nullptr)
    {
        return;
    }
    auto &self = GetByRef();
    if (self.point_light_map_.Contains(point_light_component))
    {
        Int8 index = self.point_light_map_[point_light_component];
        PointLight light;
        light.Location = point_light_component->GetWorldLocation();
        light.Color = point_light_component->GetColor();
        memcpy(&self.dynamic_global_lights_->point_lights[index], &light, sizeof(PointLight));
    }
}

void LightManager::RemoveLight(PointLightComponent *point_light_component)
{
    if (point_light_component == nullptr)
    {
        return;
    }
    auto &self = GetByRef();
    if (self.point_light_map_.Contains(point_light_component))
    {
        Int8 index = self.point_light_map_[point_light_component];
        std::swap(self.dynamic_global_lights_->point_lights[index],
                  self.dynamic_global_lights_->point_lights[self.dynamic_global_lights_->point_light_count - 1]);
        self.occupied_[self.dynamic_global_lights_->point_light_count - 1] = false;
        self.dynamic_global_lights_->point_light_count--;
    }
}

RHI::Buffer *LightManager::GetGlobalLightBuffer()
{
    auto &self = GetByRef();
    return self.global_light_buffer_.get();
}

Vector3f LightManager::GetLightPositions(PointLightComponent *Index)
{
    auto &Self = GetByRef();
    return Self.point_light_map_.begin()->first->GetWorldLocation();
}

Int8 LightManager::FindNextAvailablePointLightIndex() const
{
    for (Int8 i = 0; i < MAX_POINT_LIGHT_COUNT; ++i)
    {
        if (!occupied_[i])
        {
            return i;
        }
    }
    return -1;
}