/**
 * @file Singleton.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#pragma once

template<typename T>
class Singleton {
    friend T;
public:
    static T& Get();
};

template<typename T>
T& Singleton<T>::Get() {
    static T Instance;
    return Instance;
}
