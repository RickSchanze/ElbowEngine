/**
 * @file Singleton.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton {
public:
    static T& Get();
};

template<typename T>
T& Singleton<T>::Get() {
    static T Instance;
    return Instance;
}

#endif   //SINGLETON_H
