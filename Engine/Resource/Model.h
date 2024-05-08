/**
 * @file Model.h
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "Path/Path.h"
#include "ResourceCommon.h"
#include "Vertex.h"

RESOURCE_NAMESPACE_BEGIN

class Model {
public:
    Model(const Path& InModelPath);

    const Array<Vertex>& GetVertices() const { return mVertices; }
    const Array<uint32>& GetIndices() const { return mIndices; }
    bool                 IsValid() const { return !mVertices.empty() && !mIndices.empty(); }

private:
    Array<Vertex> mVertices;
    Array<uint32> mIndices;

    Path mPath;
};

RESOURCE_NAMESPACE_END
