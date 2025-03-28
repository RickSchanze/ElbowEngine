//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/Reflection/ReflManager.hpp"
#include "Core/String/String.hpp"


struct Any;
class Archive {
public:
    virtual ~Archive() = default;

    virtual bool Serialize(const Any &obj, String &out) = 0;

    /// 将source内容反序列化为Type对象, 放在target位置
    /// target必须不为nullptr
    virtual bool Deserialize(StringView source, void *target, const Type *type) = 0;

    template<typename T>
    T *DeserializePtr(StringView source) {
        const Type *type = TypeOf<T>();
        return static_cast<T *>(DeserializePtr(source, type));
    }

    void *DeserializePtr(StringView source, const Type *type);
};
