/**
 * @file WindowManager.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Singleton/Singleton.h"
#include "ToolCommon.h"

namespace Tool::Window
{
class WindowBase;
}

WINDOW_NAMESPACE_BEGIN

// 存储所有Window Object
// @Note: 存储的所有对象最后由ObjectManager清理
class WindowManager : public Singleton<WindowManager>
{
public:
    // 根据Type获取一个Window
    // 所有Window都是单例的
    // 如果有type则返回那个单例，没有则创建一个新的
    static WindowBase* GetOrCreateWindow(const Type& type);

    // 销毁Type对应的Window
    static void DestroyWindow(const Type& type);

    template<typename T>
        requires std::derived_from<T, WindowBase>
    static T* GetOrCreateWindow()
    {
        Type WindowType     = TypeOf<T>();
        auto WindowInstance = GetOrCreateWindow(WindowType);
        return dynamic_cast<T*>(WindowInstance);
    }

    /**
     * 获取所有非singleton的已存在的window(包括继承)
     * @param type window的type
     * @return
     */
    TArray<WindowBase*> GetWindows(const Type& type) const;

    template <typename T>
    TArray<T*> GetWindows()
    {
        TArray<T*> rtn;
        Type window_type = TypeOf<T>();
        for (auto window : GetWindows(window_type))
        {
            rtn.push_back(static_cast<T*>(window));
        }
        return rtn;
    }

    void DrawVisibleWindows(float delta_time) const;

    void AddVisibleWindow(WindowBase* window);

    void RemoveVisibleWindow(WindowBase* window);

protected:
    THashMap<Type, WindowBase*> singleton_windows_;
    TArray<WindowBase*>         duplicated_windows_;

    TArray<WindowBase*> visible_windows_;
};

WINDOW_NAMESPACE_END