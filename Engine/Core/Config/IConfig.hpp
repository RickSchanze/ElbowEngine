//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/Core.hpp"

class IConfig {
public:
    typedef IConfig ThisClass;
    virtual ~IConfig() = default;

    static Type *ConstructType() {
        const auto t = Type::Create<IConfig>("CoreConfig");
        return t;
    }

    static void ConstructSelf(void *self) { new (self) IConfig(); }
    static void DestructSelf(void *self) { static_cast<IConfig *>(self)->~IConfig(); }
    virtual const Type *GetType() const { return TypeOf<IConfig>(); }
    static const Type *GetStaticType() { return TypeOf<IConfig, true>(); }

    [[nodiscard]] bool IsDirty() const { return dirty_; }

protected:
    void SetDirty(const bool dirty) { dirty_ = dirty; }

    bool dirty_ = false;
};

struct Trigger {
    Trigger() { ReflManager::GetByRef().Register<IConfig>(); }
};
static inline Trigger _;
