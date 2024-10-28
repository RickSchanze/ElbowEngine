/**
 * @file ContainerView.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include <any>

#include "Any.h"
#include "Log/CoreLogCategory.h"
#include "Log/Logger.h"

namespace core
{

enum class ContainerType
{
    Sequential,
    Associative,
    Count,
};

class ContainerView
{
public:
    virtual ~ContainerView() = default;

    virtual void BeginIterate()     = 0;
    virtual void Next()             = 0;
    virtual void EndIterate()       = 0;
    virtual bool HasNext()          = 0;
    virtual void SetInstance(void*) = 0;

    virtual Any GetCurrentElement()
    {
        NEVER_ENTRY_WARN(LogCat::Reflection);
        return {};
    }

    virtual Any GetCurrentKey()
    {
        NEVER_ENTRY_WARN(LogCat::Reflection);
        return {};
    }

    virtual Any GetCurrentValue()
    {
        NEVER_ENTRY_WARN(LogCat::Reflection);
        return {};
    }

    virtual Any GetElementAt(int32_t index)
    {
        NEVER_ENTRY_WARN(LogCat::Reflection);
        return {};
    }

    void ForEach(const Function<void(Any)>& Func);

    void ForEach(const Function<void(Any, Any)>& Func);

    virtual ContainerType GetContainerType() = 0;
    virtual const Type*   GetElementType()   = 0;
    virtual const Type*   GetOuterType()     = 0;
    virtual int32_t       Size()             = 0;
};

template<typename ClassT, typename T>
class SequentialContainerView : public ContainerView
{
public:
    static_assert(!std::is_reference_v<T>, "T must not be a reference, could be Ref<T> instead");
    using Iterator = typename Array<T>::iterator;

    explicit SequentialContainerView(Array<T> ClassT::*container, const Type* outer) :
        container_(container), element_type_(TypeOf<T>()), outer_(outer)
    {
    }

    void BeginIterate() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
        }
        iter_     = (instance_->*container_).begin();
        size_     = (instance_->*container_).size();
        iter_cnt_ = 1;
    }

    void EndIterate() override
    {
        iter_     = {};
        size_     = 0;
        iter_cnt_ = 0;
    }

    void Next() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return;
        }
        ++iter_;
        ++iter_cnt_;
    }

    bool HasNext() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return false;
        }
        return iter_cnt_ < size_;
    }

    void SetInstance(void* instance) override { instance_ = static_cast<ClassT*>(instance); }

    Any GetCurrentElement() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return false;
        }
        return MakeRef(*iter_);
    }

    ContainerType GetContainerType() override { return ContainerType::Sequential; }

    const Type* GetElementType() override { return element_type_; }
    const Type* GetOuterType() override { return outer_; }

    Any GetElementAt(int32_t index) override
    {
        DebugAssert(LogCat::Reflection, index >= 0 && index < Size(), "Index out of range: index: {}, size: {}", index, Size());
        Iterator it = (instance_->*container_).begin();
        for (int32_t i = 0; i < index; ++i)
        {
            ++it;
        }
        return MakeRef(*it);
    }

    int32_t Size() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return 0;
        }
        return (instance_->*container_).size();
    }

private:
    ClassT* instance_ = nullptr;

    Array<T> ClassT::*container_ = nullptr;

    Iterator    iter_;
    size_t      iter_cnt_;
    size_t      size_;
    const Type* element_type_{};
    const Type* outer_{};
};
}   // namespace core
