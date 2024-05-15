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
        : mfunction(Move(func)), mId(Move(id)), mValid(true)
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
    TDelegate(String id, ObjectType *object, FuncType func) : mId(Move(id))
    {
        mfunction = [object, func](Args... args) { (object->*func)(args...); };
        mValid = true;
    }

    /**
     * 构造一个Delegate,id由GUID生成
     * @param func
     */
    explicit TDelegate(TFunction<void(Args...)> func) : mId(Guid().ToString())
    {
        mfunction = Move(func);
        mValid = true;
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
    TDelegate(ObjectType *object, FuncType func) : mId(Guid().ToString())
    {
        mfunction = [object, func](Args... args) { (object->*func)(args...); };
        mValid = true;
    }

    /**
     * 移动构造 重新生成ID并将原来的设为无效
     * @param other
     */
    TDelegate(TDelegate &&other) noexcept
    {
        mfunction = Move(other.mfunction);
        mId = Guid().ToString();
        mValid = other.mValid;
        other.mValid = false;
    }

    /**
     * 复制构造，会重新生成id
     * @param other
     */
    TDelegate(const TDelegate &other)
    {
        mfunction = other.mfunction;
        mId = Guid().ToString();
        mValid = other.mValid;
    }

    template <class... InvokeArgs>
    void operator()(InvokeArgs &&...args) const
    {
        if (mValid)
        {
            if constexpr (sizeof...(InvokeArgs) == 0)
            {
                mfunction();
            }
            else
            {
                mfunction(Forward<InvokeArgs...>(args...));
            }
        }
        else
        {
            LOG_WARNING_CATEGORY(Event, L"失效的Delegate: {}", mId);
        }
    }

    auto operator<=>(const TDelegate &rhs) const
    {
        return mId <=> rhs.mId;
    }

    /** 获取Delegate id */
    String GetID() const
    {
        return mId;
    }

    /** 此Delegate是否有效 */
    bool IsValid() const
    {
        return mValid;
    }

private:
    TFunction<void(Args...)> mfunction;
    String mId;
    bool mValid;
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
    void AddListener(DelegateType &&delegate)
    {
        mEventListener.emplace_back(Forward<DelegateType>(delegate));
    }

    /** 添加Delegate */
    template <typename ObjectType, typename ClassFunc>
    void AddObjectListener(String id, ObjectType *obj, ClassFunc func)
    {
        mEventListener.emplace_back(id, obj, func);
    }

    /** 添加Delegate */
    template <typename ObjectType, typename ClassFunc>
        requires(!std::is_same_v<ObjectType, const char>)
    void AddObjectListener(ObjectType *obj, ClassFunc func)
    {
        mEventListener.emplace_back(obj, func);
    }

    /** 添加Delegate */
    template <typename Func>
    void AddListener(String id, Func func)
    {
        mEventListener.emplace_back(Move(id), func);
    }

    /** 添加Delegate */
    void RemoveListener(String id)
    {
        auto it = std::find_if(mEventListener.begin(), mEventListener.end(),
                               [&id](const TDelegate<Args...> &delegate) { return delegate.GetName() == id; });
        if (it != mEventListener.end())
        {
            mEventListener.erase(it);
        }
    }

    /** 清除所有Delegate */
    void ClearListener()
    {
        mEventListener.clear();
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
        for (auto &listener : mEventListener)
        {
            if (listener.IsValid())
            {
                if constexpr (sizeof...(InvokeArgs) == 0)
                {
                    listener();
                }
                else
                {
                    listener(Forward<InvokeArgs...>(args...));
                }
            }
            else
            {
                LOG_WARNING_CATEGORY(Event, L"失效的事件监听者: ", listener.GetID());
            }
        }
    }

    [[nodiscard]] size_t Size() const { return mEventListener.size(); }

private:
    TArray<TDelegate<Args...>> mEventListener;
};
