/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#pragma once

#include "Core/Base/EString.h"
#include "Core/CoreDef.h"
#include "Core/Reflection/ITypeGetter.h"
#include "Core/Reflection/MetaInfoMacro.h"

#include "Core.Object.generated.h"

namespace core
{

enum ObjectCategory
{
    PureObject,   // 纯粹的对象
    GameObject,   // 在游戏世界运行
    Component,    // 这个Object是一个Component
    Window,       // 这个是一个窗口对象
    Setting,      // 这个是一个设置对象
};

class CLASS() Object : implements ITypeGetter
{
    GENERATED_CLASS(Object)
public:
    typedef Object ThisClass;

    Object();
    Object(const ThisClass&) = delete;

    Object& operator=(const ThisClass&) = delete;

    ~Object() override;

#if REGION(对象ID)
    typedef int32_t        ObjectID;
    static inline ObjectID s_id_counter_ = 0;

public:
    [[nodiscard]] ObjectID GetID() const { return id_; }

private:
    void GeneratedID();

    PROPERTY()
    ObjectID id_ = 0;
#endif

    /**
     * 获取反射类型
     * @return rttr::type
     */
    // [[nodiscard]] Type GetType() const { return get_type(); }

public:
    /**
     * 获取对象名字
     * @return
     */
    [[nodiscard]] String GetName() const { return name_; }

    /**
     * 获取对象的字符串表示
     * @return
     */
    [[nodiscard]] virtual String ToString() const;

    /**
     * 设置对象的名字
     * @param name
     */
    void SetName(const String& name);

    [[nodiscard]] bool IsComponent() const { return flag_ == Component; }

    [[nodiscard]] bool IsGameObject() const { return flag_ == GameObject; }

    // TODO: 位操作
    [[nodiscard]] ObjectCategory GetObjectCategory() const { return flag_; }


protected:
    PROPERTY()
    String name_;   // 对象名字

    PROPERTY()
    ObjectCategory flag_;
};

template<typename T>
concept IsObject = std::is_base_of_v<Object, T>;
}
