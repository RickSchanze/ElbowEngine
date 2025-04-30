//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Object/Object.hpp"
#include "Core/TypeAlias.hpp"

struct Transform;
class SceneComponent;
namespace RHI
{
class Buffer;
}

class GlobalObjectInstancedDataBuffer : public Manager<GlobalObjectInstancedDataBuffer>
{
public:
    virtual Float GetLevel() const override
    {
        return 8;
    }
    virtual StringView GetName() const override
    {
        return "GlobalObjectInstancedDataBuffer";
    }

    static RHI::Buffer* GetBuffer();

    virtual void Startup() override;
    virtual void Shutdown() override;

    static void UpdateInstancedData(ObjectHandle obj_handle, Vector3f location, Vector3f rotation, Vector3f scale);
    static void UpdateInstancedData(ObjectHandle object_handle, const Transform& transform);
    static void RemoveInstancedData(ObjectHandle handle);
    static UInt32 GetObjectInstanceIndex(ObjectHandle object_handle);
    static const Map<ObjectHandle, UInt32>& GetInstancedDataMap();

private:
    UInt32 FindNextAvailableIndex();

    SharedPtr<RHI::Buffer> buffer_;
    UInt8* mapped_memory_ = nullptr;
    Map<ObjectHandle, UInt32> mInstancedDataMap; // 每个Object对应哪个Index
    Array<UInt32> occupied_indices_;
};
