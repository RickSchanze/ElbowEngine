/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#include "Image.h"

#include "Core/Base/Base.h"
#include "Core/Utils/HashUtils.h"

using namespace platform::rhi;
using namespace core;

ImageDesc ImageDesc::Default()
{
    return ImageDesc{0, 0, IUB_Max, Format::Count};
}

size_t SamplerDesc::GetHashCode() const noexcept
{
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

size_t std::hash<SamplerDesc>::operator()(const SamplerDesc& desc) const noexcept
{
    return desc.GetHashCode();
}