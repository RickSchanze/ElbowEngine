/**
 * @file Vertex.h
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "ResourceCommon.h"


#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

RESOURCE_NAMESPACE_BEGIN

struct Vertex
{
    glm::vec3 Pos;
    glm::vec2 UV;
    bool operator==(const Vertex& Other) const {
        return Pos == Other.Pos && UV == Other.UV;
    }

};

RESOURCE_NAMESPACE_END

template<>
struct std::hash<Resource::Vertex>
{
    size_t operator()(Resource::Vertex const& vertex) const noexcept {
        return ((hash<glm::vec3>()(vertex.Pos) ^ (hash<glm::vec2>()(vertex.UV) << 1)));
    }
};   // namespace std
