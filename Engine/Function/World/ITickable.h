/**
 * @file ITickable.h
 * @author Echo 
 * @Date 24-10-6
 * @brief 
 */

#pragma once

namespace function
{
/**
 * 可Tick对象
 */
class ITickable {
public:
    virtual ~ITickable() = default;

    virtual void PreTick() {}
    virtual void Tick() {}
    virtual void PostTick() {}
};
}
