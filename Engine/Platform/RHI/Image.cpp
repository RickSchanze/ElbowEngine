/**
 * @file Image.cpp
 * @author Echo
 * @Date 24-11-24
 * @brief
 */

#include "Image.h"

#include "Core/Base/Base.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Utils/HashUtils.h"

using namespace platform::rhi;
using namespace core;

ImageDesc ImageDesc::Default() { return ImageDesc{0, 0, IUB_Max, Format::Count}; }

UInt32 Image::GetNumChannels() const {
  PROFILE_SCOPE_AUTO;
  Format f = GetFormat();
  if (f == Format::Count)
    return 0;
  switch (f) {
  case Format::D32_Float:
  case Format::R8_SRGB:
  case Format::R8_UNorm:
    return 1;
  case Format::R32G32B32_Float:
    return 3;
  case Format::R8G8B8A8_UNorm:
  case Format::B8G8R8A8_UNorm:
  case Format::R8G8B8A8_SRGB:
    return 4;
  default:
    return 0;
  }
}

size_t SamplerDesc::GetHashCode() const noexcept {
  size_t hash = 0;
  hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(GetEnumValue(mag)));
  hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(GetEnumValue(min)));
  hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(GetEnumValue(u)));
  hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(GetEnumValue(v)));
  hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(GetEnumValue(w)));
  hash = HashUtils::CombineHash(hash, std::hash<Bool>{}(enable_anisotropy));
  hash = HashUtils::CombineHash(hash, std::hash<Float>{}(max_anisotropy));
  hash = HashUtils::CombineHash(hash, std::hash<Bool>{}(unnormalized_coordinates));
  return hash;
}

size_t std::hash<SamplerDesc>::operator()(const SamplerDesc &desc) const noexcept { return desc.GetHashCode(); }