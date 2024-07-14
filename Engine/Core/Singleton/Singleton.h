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
    virtual ~Singleton() = default;

    static T* Get()
    {
        static T instance_;
        return &instance_;
    }
};
