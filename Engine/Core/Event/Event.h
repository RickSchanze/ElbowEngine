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

template <typename... Args>
class TDelegate
{
public:
    /**
     * 构造Delegate
     * @param id 自己设定的ID
     * @param func 代理的function
     */
    TDelegate(String id, TFunction<void(Args...)> func)
        : m_function(Move(func)), m_id(Move(id)), m_valid(true)
    {
    }

    /**
     * 使用一个对象的方法构造Delegate
     * @tparam ObjectType
     * @tparam FuncType
     * @param id delegate id
     * @param object 对象指针
     * @param func 对象的方法
     */
    template <typename ObjectType, typename FuncType>
    TDelegate(String id, ObjectType *object, FuncType func) : m_id(Move(id))
    {
        m_function = [object, func](Args... args) { (object->*func)(args...); };
        m_valid = true;
    }

    /**
     * 构造一个Delegate,id由GUID生成
     * @param func
     */
    explicit TDelegate(TFunction<void(Args...)> func) : m_id(Guid().ToString())
    {
        m_function = Move(func);
        m_valid = true;
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
    template <typename ObjectType, typename FuncType>
        requires(!std::is_same_v<ObjectType, const char>)
    TDelegate(ObjectType *object, FuncType func) : m_id(Guid().ToString())
    {
        m_function = [object, func](Args... args) { (object->*func)(args...); };
        m_valid = true;
    }

    /**
     * 移动构造 重新生成ID并将原来的设为无效
     * @param other
     */
    TDelegate(TDelegate &&other) noexcept
    {
        m_function = Move(other.m_function);
        m_id = Guid().ToString();
        m_valid = other.mValid;
        other.mValid = false;
    }

    /**
     * 复制构造，会重新生成id
     * @param other
     */
    TDelegate(const TDelegate &other)
    {
        m_function = other.m_function;
        m_id = Guid().ToString();
        m_valid = other.mValid;
    }

    template <class... InvokeArgs>
    void operator()(InvokeArgs &&...args) const
    {
        if (m_valid)
        {
            if constexpr (sizeof...(InvokeArgs) == 0)
            {
                m_function();
            }
            else
            {
                m_function(Forward<InvokeArgs>(args)...);
            }
        }
        else
        {
            LOG_WARNING_CATEGORY(Event, L"失效的Delegate: {}", m_id);
        }
    }

    auto operator<=>(const TDelegate &rhs) const
    {
        return m_id <=> rhs.mId;
    }

    /** 获取Delegate id */
    String GetID() const
    {
        return m_id;
    }

    /** 此Delegate是否有效 */
    bool IsValid() const
    {
        return m_valid;
    }

private:
    TFunction<void(Args...)> m_function;
    String m_id;
    bool m_valid;
};

template <typename... Args>
class TEvent
{
public:
    TEvent() = default;
    // 禁止拷贝构造和赋值
    TEvent(const TEvent &) = delete;
    TEvent &operator=(const TEvent &) = delete;

    /** 使用其他的Delegate或者Lambda来初始化添加Event */
    template <typename DelegateType>
    void Add(DelegateType &&delegate)
    {
        m_event_listener.emplace_back(Forward<DelegateType>(delegate));
    }

    /** 添加Delegate */
    template <typename ObjectType, typename ClassFunc>
    void AddObject(String id, ObjectType *obj, ClassFunc func)
    {
        m_event_listener.emplace_back(id, obj, func);
    }

    /** 添加Delegate */
    template <typename ObjectType, typename ClassFunc>
        requires(!std::is_same_v<ObjectType, const char>)
    void AddObject(ObjectType *obj, ClassFunc func)
    {
        m_event_listener.emplace_back(obj, func);
    }

    /** 添加Delegate */
    template <typename Func>
    void Add(String id, Func func)
    {
        m_event_listener.emplace_back(Move(id), func);
    }

    /** 添加Delegate */
    void Remove(String id)
    {
        auto it = std::find_if(m_event_listener.begin(), m_event_listener.end(),
                               [&id](const TDelegate<Args...> &delegate) { return delegate.GetName() == id; });
        if (it != m_event_listener.end())
        {
            m_event_listener.erase(it);
        }
    }

    /** 清除所有Delegate */
    void Clear()
    {
        m_event_listener.clear();
    }

    /**
     * 分发当前事件
     * 将会调用绑定到此事件上的所有函数
     * @tparam InvokeArgs
     * @param args
     */
    template <typename... InvokeArgs>
    void Broadcast(InvokeArgs &&...args)
    {
        for (auto &listener : m_event_listener)
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
