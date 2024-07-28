/**
 * @file WindowManager.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Singleton/Singleton.h"

namespace Tool::Window {
class WindowBase;
}

// 存储所有Window Object
// @Note: 存储的所有对象最后由ObjectManager清理
class WindowManager : public Singleton<WindowManager> {
public:
    // 根据Type获取一个Window
    // 所有Window都是单例的
    // 如果有type则返回那个单例，没有则创建一个新的
    static Tool::Window::WindowBase* GetWindow(Type type);

    // 销毁Type对应的Window
    static void DestroyWindow(const Type& type);

    template<typename T>
        requires std::derived_from<T, Tool::Window::WindowBase>
    static T* GetWindow() {
        Type WindowType     = TypeOf<T>();
        auto WindowInstance = GetWindow(WindowType);
        return dynamic_cast<T*>(WindowInstance);
    }

protected:
    THashMap<Type, Tool::Window::WindowBase*> windows_map_;
};
