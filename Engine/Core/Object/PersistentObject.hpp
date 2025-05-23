//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Object.hpp"

#include GEN_HEADER("PersistentObject.generated.hpp")

class ECLASS() PersistentObject : public Object {
    GENERATED_BODY(PersistentObject);

public:
    PersistentObject() : Object(OFB_Persistent) {}

    /**
     * 调用此函数时, handle_已经设置,
     * 根据此去加载持久化资产
     */
    virtual void PerformLoad() {}

    virtual void PerformUnload() {}

    virtual bool IsLoaded() const { return false; }

    bool IsDirty() const { return dirty_; }

    void SetDirty(bool dirty) { dirty_ = dirty; }

protected:
    bool dirty_ = false;
};
