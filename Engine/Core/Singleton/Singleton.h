/**
 * @file Singleton.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#pragma once

template<typename T>
class Singleton
{
    friend T;

public:
    static T* Get() { return instance_; }

    virtual ~Singleton() = default;

    virtual void Intialize() {}
    virtual void Finalize() {}

protected:
    static inline T* instance_;
};
