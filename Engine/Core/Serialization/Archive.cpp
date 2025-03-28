//
// Created by Echo on 2025/3/24.
//

#include "Archive.hpp"

void *Archive::DeserializePtr(StringView source, const Type *type) {
    void *target = Malloc(type->GetSize());
    ReflManager::Get()->ConstructAt(type, target);
    if (Deserialize(source, target, type)) {
        return target;
    } else {
        ReflManager::Get()->DestroyAt(type, target);
        return nullptr;
    }
}
