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
#include <glm/gtx/hash.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    bool operator==(const Vertex& Other) const { return position == Other.position && uv == Other.uv; }
};

template<>
struct std::hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const noexcept {
        return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec2>()(vertex.uv) << 1)));
    }
};   // namespace std
