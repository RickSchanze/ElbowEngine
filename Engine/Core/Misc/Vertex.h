/**
 * @file Vertex.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */
#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
    glm::vec3 Pos;
    glm::vec3 Normal;
    glm::vec2 UV;

    bool operator==(const Vertex& Other) const { return Pos == Other.Pos && UV == Other.UV; }
};

template<>
struct std::hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const noexcept {
        return ((hash<glm::vec3>()(vertex.Pos) ^ (hash<glm::vec2>()(vertex.UV) << 1)));
    }
};   // namespace std
