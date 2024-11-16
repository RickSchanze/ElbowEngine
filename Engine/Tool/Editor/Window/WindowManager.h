/**
 * @file WindowManager.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include "Core/Singleton/Singleton.h"
#include "ToolCommon.h"

namespace tool::window
{
class WindowBase;
}

namespace tool::window {

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
    Array<WindowBase*> GetWindows(const Type& type) const;

    template <typename T>
    Array<T*> GetWindows()
    {
        Array<T*> rtn;
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
    HashMap<Type, WindowBase*> singleton_windows_;
    Array<WindowBase*>         duplicated_windows_;

    Array<WindowBase*> visible_windows_;
};

}
