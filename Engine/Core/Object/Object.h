/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#ifndef ELBOWENGINE_OBJECT_H
#define ELBOWENGINE_OBJECT_H
#include "Core/CoreDef.h"

class Object {
    RTTR_ENABLE()
    RTTR_REGISTRATION_FRIEND

    friend class ObjectCreateHelper;

public:
    virtual ~Object();

    /**
     * 获取反射类型
     * @return rttr::typr
     */
    [[nodiscard]] rttr::type GetType() const { return get_type(); }

public:
    /**
     * 获取对象名字
     * @return
     */
    [[nodiscard]] String GetName() const { return mName; }

    /**
     * 获取对象的字符串表示
     * @return
     */
    [[nodiscard]] virtual String ToString() const;

    /**
     * 获取对象ID
     * @return
     */
    [[nodiscard]] uint32 GetID() const { return mID; }

    /**
     * 设置对象的名字
     * @param Name
     */
    void SetName(String Name) { mName = std::move(Name); }

    /**
     * 对象是否还有效
     * @return
     */
    [[nodiscard]] bool Valid() const;

private:
    String mName;        // 对象名字
    uint32 mID;          // 对象ID
    bool   mIsGarbage;   // 对象是否应该被清除
};

template <typename T>
concept IsObject = std::is_base_of_v<Object, T>;

#endif   //ELBOWENGINE_OBJECT_H
