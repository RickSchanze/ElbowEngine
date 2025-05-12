#pragma once
#include "Core/Assert.hpp"
#include "Core/TypeAlias.hpp"
#include "Core/TypeTraits.hpp"

static inline UInt64 g_delegate_id_counter = 0;

// TDelegate是一个带Id的TFunction wrapper
template <typename ReturnT, typename... ArgumentTypes>
struct Delegate
{
    using ReturnType = ReturnT;

    explicit Delegate(Function<ReturnT(ArgumentTypes...)>&& function) : function_(Move(function)), id_(++g_delegate_id_counter)
    {
    }

    Delegate() = default;

    Delegate(Delegate&& other) noexcept : function_(Move(other.function_)), id_(other.id_)
    {
        other.id_ = 0;
    }
    Delegate(const Delegate& other) : function_(other.function_), id_(other.id_)
    {
    }

    Delegate& operator=(Delegate&& other) noexcept
    {
        if (this != &other)
        {
            function_ = Move(other.function_);
            id_ = other.id_;
            other.id_ = 0;
        }
        return *this;
    }

    bool operator==(const Delegate& other) const
    {
        return id_ == other.id_;
    }

    UInt64 GetID() const
    {
        return id_;
    }

    void Unbind()
    {
        function_ = {};
        id_ = 0;
    }

    bool IsValid() const
    {
        return id_ != 0;
    }
    bool HasBound() const
    {
        return static_cast<bool>(function_);
    }

    void Bind(Function<ReturnT(ArgumentTypes...)>&& function)
    {
        DebugAssert(!HasBound(), "Delegate is already bound, rebinding...");
        function_ = Move(function);
        // 重新绑定进行绑定则生成新的id
        id_ = ++g_delegate_id_counter;
    }

    template <typename... Args>
        requires NTraits::CanParameterPackConvert<Tuple<Args...>, Tuple<ArgumentTypes...>>
    ReturnT Invoke(Args&&... args)
    {
        if (HasBound())
        {
            return function_(Forward<Args>(args)...);
        }
        else
        {
            Log(Warn) << String::Format("Delegate {} is not bound, invoke failed. return {{}} if there needs a return value.", GetID());
            if constexpr (!NTraits::SameAs<ReturnT, void>)
            {
                return {};
            }
        }
    }

private:
    Function<ReturnT(ArgumentTypes...)> function_{};
    UInt64 id_{0};
};

template <typename ReturnT, typename... ArgumentArgs>
struct Event
{
    using ReturnType = ReturnT;
    using DelegateType = Delegate<ReturnType, ArgumentArgs...>;

    Event() : mDelegate()
    {
    }

    void Bind(DelegateType&& delegate)
    {
        DebugAssert(!HasBound(), "Event has been bound, rebinding.");
        mDelegate = Move(delegate);
    }

    void Bind(ReturnType (*func)(ArgumentArgs...))
    {
        DebugAssert(!HasBound(), "Event has been bound, rebinding.");
        mDelegate = Move(DelegateType(func));
    }

    template <typename Class>
    void Bind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...))
    {
        DebugAssert(!HasBound(), "Event has been bound, rebinding.");
        mDelegate = Move(DelegateType([obj, func](ArgumentArgs&&... args) { return (obj->*func)(Move(args)...); }));
    }

    template <typename Class>
    void Bind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...) const)
    {
        DebugAssert(!HasBound(), "Event has been bound, rebinding.");
        mDelegate = Move(DelegateType([obj, func](ArgumentArgs&&... args) { return (obj->*func)(Move(args)...); }));
    }

    void Unbind()
    {
        mDelegate.Unbind();
    }

    bool HasBound() const
    {
        return mDelegate.HasBound();
    }

    template <typename... Args>
        requires NTraits::CanParameterPackConvert<Tuple<Args...>, Tuple<ArgumentArgs...>>
    ReturnType Invoke(Args&&... args)
    {
        return mDelegate.Invoke(Forward<Args>(args)...);
    }

    ReturnType InvokeOnce(ArgumentArgs&&... args)
    {
        if constexpr (NTraits::SameAs<ReturnT, void>)
        {
            mDelegate.Invoke(Forward<ArgumentArgs>(args)...);
            Unbind();
            return;
        }
        else
        {
            ReturnType ret = mDelegate.Invoke(Move(args)...);
            Unbind();
            return ret;
        }
    }

private:
    DelegateType mDelegate;
};

/**
 * 多播委托
 * @note 顺序是乱的, 不能让逻辑依赖绑定顺序！！！
 */
template <typename ReturnT, typename... ArgumentArgs>
struct MulticastEvent
{
    using ReturnType = ReturnT;
    using DelegateType = Delegate<ReturnType, ArgumentArgs...>;
    static_assert(NTraits::SameAs<ReturnType, void>, "MulticastEvent can not return value");

    /**
     * 为这个多播事件增加一个委托绑定
     * @note 如过传入的是一个Func, 那么DebugAssert永远不会触发,
     * DebugAssert主要用于同一个Delegate是否绑定两次,
     *       而不是同一个Callable是否绑定两次
     * @param delegate 要进行绑定的委托
     * @return
     */
    UInt64 AddBind(DelegateType&& delegate)
    {
        if (delegates_.Contains(delegate))
        {
            return delegate.GetID();
        }
        delegates_.Emplace(Move(delegate));
        return delegates_.Last().GetID();
    }

    UInt64 AddBind(ReturnType (*func)(ArgumentArgs...))
    {
        DelegateType delegate(func);
        return AddBind(Move(delegate));
    }

    template <typename Class>
    UInt64 AddBind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...))
    {
        DelegateType delegate([obj, func](ArgumentArgs... args) { return (obj->*func)(args...); });
        return AddBind(Move(delegate));
    }

    template <typename Class>
    UInt64 AddBind(Class* obj, ReturnType (Class::*func)(ArgumentArgs...) const)
    {
        DelegateType delegate([obj, func](ArgumentArgs... args) { return (obj->*func)(args...); });
        return AddBind(Move(delegate));
    }

    bool RemoveBind(UInt64 id)
    {
        for (int i = 0; i < delegates_.Count(); ++i)
        {
            if (delegates_[i].GetID() == id)
            {
                delegates_.FastRemoveAt(i);
                return true;
            }
        }
        return false;
    }

    void ClearBind()
    {
        delegates_.Clear();
    }

    template <typename... Args>
    void Invoke(Args&&... args)
        requires NTraits::CanParameterPackConvert<Tuple<Args...>, Tuple<ArgumentArgs...>>
    {
        for (auto& delegate : delegates_)
        {
            delegate.Invoke(Forward<ArgumentArgs>(args)...);
        }
    }

    template <typename... Args>
    void InvokeOnce(Args&&... args)
        requires NTraits::CanParameterPackConvert<Tuple<Args...>, Tuple<ArgumentArgs...>>
    {
        for (auto& delegate : delegates_)
        {
            delegate.Invoke(Forward<ArgumentArgs>(args)...);
        }
        ClearBind();
    }

private:
    Array<DelegateType> delegates_;
};
