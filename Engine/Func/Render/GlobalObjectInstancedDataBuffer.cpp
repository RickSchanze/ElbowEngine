//
// Created by Echo on 2025/3/27.
//

#include "GlobalObjectInstancedDataBuffer.hpp"

#include "Core/Config/ConfigManager.hpp"
#include "Core/Memory/Memory.hpp"
#include "Func/Config/FuncConfig.hpp"
#include "Func/World/Transform.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/Misc.hpp"
using namespace NRHI;

Buffer *GlobalObjectInstancedDataBuffer::GetBuffer() { return GetByRef().buffer_.get(); }

void GlobalObjectInstancedDataBuffer::Startup() {
    const auto* cfg = GetConfig<FuncConfig>();
    const UInt16 count = cfg->GetPreallocatedInstacnedDataCount() + 1; // 防止为0
    const BufferDesc desc{sizeof(InstancedData1) * count, BUB_VertexBuffer, BMPB_HostVisible | BMPB_HostCoherent};
    buffer_ = GetGfxContextRef().CreateBuffer(desc, "GlobalInstancedDataBuffer");
    mapped_memory_ = buffer_->BeginWrite();
    occupied_indices_.Resize(count);
}

void GlobalObjectInstancedDataBuffer::Shutdown() {
    GetGfxContext()->WaitForDeviceIdle();
    buffer_->EndWrite();
    buffer_ = nullptr;
    mapped_memory_ = nullptr;
}

void GlobalObjectInstancedDataBuffer::UpdateInstancedData(ObjectHandle obj_handle, Vector3f location, Vector3f rotation, Vector3f scale) {
    auto &self = GetByRef();
    if (self.mInstancedDataMap.Contains(obj_handle)) {
        const UInt32 index = self.mInstancedDataMap[obj_handle];
        InstancedData1 new_data;
        new_data.Location = location;
        new_data.Rotation = rotation;
        new_data.Scale = scale;
        Memcpy(self.mapped_memory_ + sizeof(InstancedData1) * index, &new_data, sizeof(InstancedData1));
    } else {
        const UInt32 new_index = self.FindNextAvailableIndex();
        self.occupied_indices_[new_index] = true;
        self.mInstancedDataMap[obj_handle] = new_index;
        InstancedData1 new_data;
        new_data.Location = location;
        new_data.Rotation = rotation;
        new_data.Scale = scale;
        Memcpy(self.mapped_memory_ + sizeof(InstancedData1) * new_index, &new_data, sizeof(InstancedData1));
    }
}

void GlobalObjectInstancedDataBuffer::UpdateInstancedData(ObjectHandle object_handle, const Transform &transform) {
    const Vector3f Location = transform.GetLocation();
    const Vector3f Rotation = transform.GetRotator();
    const Vector3f Scale = transform.GetScale();
    UpdateInstancedData(object_handle, Location, Rotation, Scale);
}

void GlobalObjectInstancedDataBuffer::RemoveInstancedData(ObjectHandle handle) {
    auto &self = GetByRef();
    self.mInstancedDataMap.Remove(handle);
}

UInt32 GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(ObjectHandle object_handle) {
    auto &self = GetByRef();
    if (self.mInstancedDataMap.Contains(object_handle)) {
        return self.mInstancedDataMap[object_handle];
    }
    return NumberMax<UInt32>();
}

const Map<ObjectHandle, UInt32> &GlobalObjectInstancedDataBuffer::GetInstancedDataMap() {
    auto &self = GetByRef();
    return self.mInstancedDataMap;
}

UInt32 GlobalObjectInstancedDataBuffer::FindNextAvailableIndex() {
    for (UInt32 i = 0; i < occupied_indices_.Count(); i++) {
        if (!occupied_indices_[i])
            return i;
    }
    return -1;
}
