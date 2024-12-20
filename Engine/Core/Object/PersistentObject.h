//
// Created by Echo on 24-12-20.
//

#pragma once
#include "Object.h"

namespace core
{
class PersistentObject : public Object
{
public:
    PersistentObject() : Object(Persistent) {}

    /**
     * 调用此函数时, handle_已经设置,
     * 根据此去加载持久化资产
     */
    virtual void PerformLoad() {}
};
}   // namespace core
