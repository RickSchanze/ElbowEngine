/**
 * @file Event.h
 * @author Echo
 * @Date 2024/2/27
 * @brief 事件系统
 */
#pragma once
#include "CoreDef.h"
#include "CoreGlobal.h"
#include "Misc/Guid.h"

// TODO: 事件系统需要重构
template<typename... Args>
class TDelegate
{
public:
    /**
     * 构造Delegate
     * @param id 自己设定的ID
     * @param func 代理的function
     */
    TDelegate(String id, TFunction<void(Args...)> func) : function_(Move(func)), id_(Move(id)), valid_(true) {}

    /**
     * 使用一个对象的方法构造Delegate
     * @tparam ObjectType
     * @tparam FuncType
     * @param id delegate id
     * @param object 对象指针
     * @param func 对象的方法
     */
    template<typename ObjectType, typename FuncType>
    TDelegate(String id, ObjectType* object, FuncType func) : id_(Move(id))
    {
        function_ = [object, func](Args... args) { (object->*func)(args...); };
        valid_    = true;
    }

    /**
     * 构造一个Delegate,id由GUID生成
     * @param func
     */
    explicit TDelegate(TFunction<void(Args...)> func) : id_(Guid().ToString())
    {
        function_ = Move(func);
        valid_    = true;
    }

    /**
     * 使用一个对象的方法构造Delegate
     *
     * @note id由GUID生成d
     *
     * @tparam ObjectType 对象类型
     * @tparam FuncType 函数类型
     * @param object 对象指针
     * @param func 函数指针
     */
    template<typename ObjectType, typename FuncType>
        requires(!std::is_same_v<ObjectType, const char>)
    TDelegate(ObjectType* object, FuncType func) : id_(Guid().ToString())
    {
        function_ = [object, func](Args... args) { (object->*func)(args...); };
        valid_    = true;
    }

    TDelegate(TDelegate&& other) noexcept
    {
        function_    = Move(other.function_);
        id_          = other.GetID();
        valid_       = other.valid_;
        other.valid_ = false;
    }

    TDelegate(const TDelegate& other)
    {
        function_ = other.function_;
        id_       = other.GetID();
        valid_    = other.valid_;
    }

    template<class... InvokeArgs>
    void operator()(InvokeArgs&&... args) const
    {
        if (valid_)
        {
            if constexpr (sizeof...(InvokeArgs) == 0)
            {
                function_();
            }
            else
            {
                function_(Forward<InvokeArgs>(args)...);
            }
        }
        else
        {
            LOG_WARNING_CATEGORY(Event, L"失效的Delegate: {}", id_);
        }
    }

    auto operator<=>(const TDelegate& rhs) const { return id_ <=> rhs.id_; }

    /** 获取Delegate id */
    String GetID() const { return id_; }

    /** 此Delegate是否有效 */
    bool IsValid() const { return valid_; }

private:
    TFunction<void(Args...)> function_;
    String                   id_;
    bool                     valid_;
};

template<typename... Args>
class TEvent
{
public:
    TEvent() = default;

    // 禁止拷贝构造和赋值
    TEvent(const TEvent&) = delete;

    TEvent& operator=(const TEvent&) = delete;

    /** 使用其他的Delegate或者Lambda来初始化添加Event */
    template<typename DelegateType>
    String Add(DelegateType&& delegate)
    {
        TDelegate<Args...> new_delegate(Move(delegate));
        m_event_listener.emplace_back(Move(new_delegate));
        return new_delegate.GetID();
    }

    /** 添加Delegate */
    template<typename ObjectType, typename ClassFunc>
    String AddObject(String id, ObjectType* obj, ClassFunc func)
    {
        TDelegate<Args...> new_delegate(id, obj, func);
        m_event_listener.emplace_back(Move(new_delegate));
        return new_delegate.GetID();
    }

    /** 添加Delegate */
    template<typename ObjectType, typename ClassFunc>
        requires(!std::is_same_v<ObjectType, const char>)
    String AddObject(ObjectType* obj, ClassFunc func)
    {
        TDelegate<Args...> new_delegate(obj, func);
        m_event_listener.emplace_back(Move(new_delegate));
        return new_delegate.GetID();
    }

    /** 添加Delegate */
    template<typename Func>
    String Add(String id, Func func)
    {
        TDelegate<Args...> new_delegate(id, Move(func));
        m_event_listener.emplace_back(Move(new_delegate));
        return new_delegate.GetID();
    }

    /** 添加Delegate */
    void Remove(String id)
    {
        auto it = std::find_if(m_event_listener.begin(), m_event_listener.end(), [&id](const TDelegate<Args...>& delegate) {
            return delegate.GetID() == id;
        });
        if (it != m_event_listener.end())
        {
            m_event_listener.erase(it);
        }
    }

    /** 清除所有Delegate */
    void Clear() { m_event_listener.clear(); }

    /**
     * 分发当前事件
     * 将会调用绑定到此事件上的所有函数
     * @tparam InvokeArgs
     * @param args
     */
    template<typename... InvokeArgs>
    void Broadcast(InvokeArgs&&... args)
    {
        for (auto& listener: m_event_listener)
        {
            if (listener.IsValid())
            {
                if constexpr (sizeof...(InvokeArgs) == 0)
                {
                    listener();
                }
                else
                {
                    listener(Forward<InvokeArgs>(args)...);
                }
            }
            else
            {
                LOG_WARNING_CATEGORY(Event, L"失效的事件监听者: ", listener.GetID());
            }
        }
    }

    [[nodiscard]] size_t Size() const { return m_event_listener.size(); }

private:
    TArray<TDelegate<Args...>> m_event_listener;
};
