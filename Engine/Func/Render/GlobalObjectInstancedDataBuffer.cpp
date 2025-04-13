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
using namespace RHI;

Buffer *GlobalObjectInstancedDataBuffer::GetBuffer() { return GetByRef().buffer_.get(); }

void GlobalObjectInstancedDataBuffer::Startup() {
    auto *cfg = GetConfig<FuncConfig>();
    UInt16 count = cfg->GetPreallocatedInstacnedDataCount() + 1; // 防止为0
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
    if (self.instanced_data_map_.Contains(obj_handle)) {
        UInt32 index = self.instanced_data_map_[obj_handle];
        InstancedData1 new_data;
        new_data.location = location;
        new_data.rotation = rotation;
        new_data.scale = scale;
        Memcpy(self.mapped_memory_ + sizeof(InstancedData1) * index, &new_data, sizeof(InstancedData1));
    } else {
        UInt32 new_index = self.FindNextAvailableIndex();
        self.occupied_indices_[new_index] = true;
        self.instanced_data_map_[obj_handle] = new_index;
        InstancedData1 new_data;
        new_data.location = location;
        new_data.rotation = rotation;
        new_data.scale = scale;
        Memcpy(self.mapped_memory_ + sizeof(InstancedData1) * new_index, &new_data, sizeof(InstancedData1));
    }
}

void GlobalObjectInstancedDataBuffer::UpdateInstancedData(ObjectHandle object_handle, const Transform &transform) {
    Vector3f location = transform.location;
    Vector3f rotation = transform.GetRotationEuler();
    Vector3f scale = transform.scale;
    UpdateInstancedData(object_handle, location, rotation, scale);
}

void GlobalObjectInstancedDataBuffer::RemoveInstancedData(ObjectHandle handle) {
    auto &self = GetByRef();
    self.instanced_data_map_.Remove(handle);
}

UInt32 GlobalObjectInstancedDataBuffer::GetObjectInstanceIndex(ObjectHandle object_handle) {
    auto &self = GetByRef();
    if (self.instanced_data_map_.Contains(object_handle)) {
        return self.instanced_data_map_[object_handle];
    }
    return NumberMax<UInt32>();
}

const Map<ObjectHandle, UInt32> &GlobalObjectInstancedDataBuffer::GetInstancedDataMap() {
    auto &self = GetByRef();
    return self.instanced_data_map_;
}

UInt32 GlobalObjectInstancedDataBuffer::FindNextAvailableIndex() {
    for (UInt32 i = 0; i < occupied_indices_.Count(); i++) {
        if (!occupied_indices_[i])
            return i;
    }
    return -1;
}
