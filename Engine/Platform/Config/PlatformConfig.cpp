//
// Created by Echo on 2025/3/25.
//
#include "PlatformConfig.hpp"

#include "Core/Core.hpp"
bool PlatformConfig::GetValidEnableValidationLayer() const {
#ifdef ELBOW_DEBUG
    return enable_validation_layer;
#else
    return false;
#endif
}

UInt8 PlatformConfig::GetValidFrameCountInFlight() const { return std::clamp(GetFrameCountInFlight(), static_cast<UInt8>(1), static_cast<UInt8>(8)); }
