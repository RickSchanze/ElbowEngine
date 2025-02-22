/**
 * @file PlatformConfig.cpp
 * @author Echo
 * @Date 24-11-19
 * @brief
 */

#include "PlatformConfig.h"

#include "Core/Math/Math.h"
#include GEN_HEADER("Platform.PlatformConfig.generated.h")

using namespace platform;

GENERATED_SOURCE()

uint8_t PlatformConfig::GetValidFrameCountInFlight() const {
  return std::clamp(GetFrameCountInFlight(), static_cast<uint8_t>(1), static_cast<uint8_t>(8));
}

bool PlatformConfig::GetValidEnableValidationLayer() const {
#ifdef ELBOW_DEBUG
  return GetEnableValidationLayer();
#else
  return false;
#endif
}
