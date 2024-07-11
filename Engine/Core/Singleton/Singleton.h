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
    static T* Get()
    {
        if (instance_ == nullptr)
        {
            instance_ = new T();
        }
        return instance_;
    }

    virtual ~Singleton()
    {
        delete instance_;
        instance_ = nullptr;
    }


protected:
    static inline T* instance_;
};
