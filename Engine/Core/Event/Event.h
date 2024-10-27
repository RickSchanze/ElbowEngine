/**
 * @file Event.h
 * @author Echo 
 * @Date 24-10-20
 * @brief 
 */

#pragma once

#include "CoreTypeTraits.h"
#include "Delegate.h"
#include "Utils/ContainerUtils.h"

// TODO: AsyncEvent AsyncMultiCastEvent

namespace core
{
template<typename ReturnT, typename... ArgumentArgs>
struct Event
{
    using ReturnType   = ReturnT;
    using DelegateType = Delegate<ReturnType, ArgumentArgs...>;

    Event() : delegate_() {}

    void Bind(DelegateType&& delegate)
    {
        // DebugAssert(Event, HasBound(), "Event has been bound, rebinding.");
        delegate_ = Move(delegate);
    }

    void Bind(ReturnType (*func)(ArgumentArgs...))
    {
        // DebugAssert(Event, !HasBound(), "Event has been bound, rebinding.");
        delegate_ = Move(DelegateType(func));
    }

    template<typename Class>
    void Bind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...))
    {
        // DebugAssert(Event, !HasBound(), "Event has been bound, rebinding.");
        delegate_ = Move(DelegateType([obj, func](ArgumentArgs&&... args) { return (obj->*func)(Move(args)...); }));
    }

    template<typename Class>
    void Bind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...) const)
    {
        // DebugAssert(Event, !HasBound(), "Event has been bound, rebinding.");
        delegate_ = Move(DelegateType([obj, func](ArgumentArgs&&... args) { return (obj->*func)(Move(args)...); }));
    }

    void Unbind() { delegate_.Unbind(); }

    [[nodiscard]] bool HasBound() const { return delegate_.HasBound(); }

    ReturnType Invoke(ArgumentArgs&&... args) { return delegate_.Invoke(Move(args)...); }

    ReturnType InvokeOnce(ArgumentArgs&&... args)
    {
        if constexpr (::std::is_same_v<ReturnT, void>)
        {
            delegate_.Invoke(Move(args)...);
            Unbind();
            return;
        }
        else
        {
            ReturnType ret = delegate_.Invoke(Move(args)...);
            Unbind();
            return ret;
        }
    }

private:
    DelegateType delegate_;
};

/**
 * 多播委托
 * @note 顺序是乱的, 不能让逻辑依赖绑定顺序！！！
 */
template<typename ReturnT, typename... ArgumentArgs>
struct MulticastEvent
{
    using ReturnType   = ReturnT;
    using DelegateType = Delegate<ReturnType, ArgumentArgs...>;
    static_assert(::std::is_same_v<ReturnType, void>, "MulticastEvent can not return value");

    /**
     * 为这个多播事件增加一个委托绑定
     * @note 如过传入的是一个Func, 那么DebugAssert永远不会触发, DebugAssert主要用于同一个Delegate是否绑定两次,
     *       而不是同一个Callable是否绑定两次
     * @param delegate 要进行绑定的委托
     * @return
     */
    DelegateID AddBind(DelegateType&& delegate)
    {
        // DebugAssert(Event, !ContainerUtils::Contains(delegates_, delegate), "Event has already bind delegate {}", delegate.GetID());
        delegates_.emplace_back(Move(delegate));
        return delegates_.back().GetID();
    }

    DelegateID AddBind(ReturnType (*func)(ArgumentArgs...))
    {
        DelegateType delegate(func);
        return AddBind(Move(delegate));
    }

    template<typename Class>
    DelegateID AddBind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...))
    {
        DelegateType delegate([obj, func](ArgumentArgs... args) { return (obj->*func)(args...); });
        return AddBind(Move(delegate));
    }

    template<typename Class>
    DelegateID AddBind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...) const)
    {
        DelegateType delegate([obj, func](ArgumentArgs... args) { return (obj->*func)(args); });
        return AddBind(Move(delegate));
    }

    bool RemoveBind(DelegateID id)
    {
        for (int i = 0; i < delegates_.size(); ++i)
        {
            if (delegates_[i].GetID() == id)
            {
                ContainerUtils::FastRemoveAt(delegates_, i);
                return true;
            }
        }
        return false;
    }

    void ClearBind() { delegates_.clear(); }

    template<typename... Args>
    void Invoke(Args&&... args)
        requires CanParameterPackConvert<::std::tuple<Args...>, ::std::tuple<ArgumentArgs...>>::Value
    {
        for (auto& delegate: delegates_)
        {
            delegate.Invoke(Forward<ArgumentArgs>(args)...);
        }
    }

    template<typename... Args>
    void InvokeOnce(Args&&... args)
        requires CanParameterPackConvert<::std::tuple<Args...>, ::std::tuple<ArgumentArgs...>>::Value
    {
        for (auto& delegate: delegates_)
        {
            delegate.Invoke(Forward<ArgumentArgs>(args)...);
        }
        ClearBind();
    }

private:
    Array<DelegateType> delegates_;
};
}   // namespace core

// 定义一个事件可以带有返回值
#define DECLARE_EVENT(name, return_type, ...)            \
    struct name : public core::Event<return_type, __VA_ARGS__> \
    {                                                    \
    };

// 定义一个多播事件, 注意: 不能有返回值
#define DECLARE_MULTICAST_EVENT(name, ...) \
    struct name : public core::MulticastEvent<void, __VA_ARGS__> {};
