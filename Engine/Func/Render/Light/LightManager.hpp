//
// Created by Echo on 2025/4/5.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Object/Object.hpp"
#include "Platform/RHI/Misc.hpp"


namespace RHI {
    class Buffer;
}
class PointLightComponent;

constexpr auto MAX_POINT_LIGHT_COUNT = 8;

struct DynamicGlobalLights {
    RHI::PointLight point_lights[MAX_POINT_LIGHT_COUNT];
    UInt8 _padding[256 - MAX_POINT_LIGHT_COUNT * sizeof(RHI::PointLight)];
    Int32 point_light_count{};
};

class LightManager : public Manager<LightManager> {
public:
    [[nodiscard]] Float GetLevel() const override { return 11.2f; }
    [[nodiscard]] StringView GetName() const override { return "LightManager"; }

    void Startup() override;
    void Shutdown() override;

    static void AddLight(PointLightComponent *point_light_component);
    static void UpdateLight(PointLightComponent *point_light_component);
    static void RemoveLight(PointLightComponent *point_light_component);
    static RHI::Buffer* GetGlobalLightBuffer();

private:
    Int8 FindNextAvailablePointLightIndex() const;

    Map<PointLightComponent *, Int8> point_light_map_; // 由点光源组件映射至点光源Index
    DynamicGlobalLights* dynamic_global_lights_ = nullptr;
    SharedPtr<RHI::Buffer> global_light_buffer_;
    Bool occupied_[MAX_POINT_LIGHT_COUNT] = {};
};
