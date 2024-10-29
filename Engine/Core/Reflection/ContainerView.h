/**
 * @file ContainerView.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include <any>

#include "Any.h"
#include "Base/CoreTypeDef.h"
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

/**
 * 用于反射容器的类, 此类可以迭代, 可以获取元素
 * 但是(暂时)无法增大、减小元素
 * TODO: 完整的增删改查接口
 */
class ContainerView
{
public:
    virtual ~ContainerView() = default;

    virtual bool BeginIterate()     = 0;
    virtual void Next()             = 0;
    virtual void EndIterate()       = 0;
    virtual bool HasNext()          = 0;
    virtual void SetInstance(void*) = 0;

#if REGION(顺序容器)
    virtual const Type* GetElementType()
    {
        NEVER_ENTRY_WARN(LogCat::Reflection);
        return {};
    }

    virtual Any GetCurrentElement()
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
#endif

#if REGION(关联容器)
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
#endif

    void ForEach(const Function<void(Any, Any)>& Func);

    virtual ContainerType GetContainerType() = 0;

    virtual const Type* GetOuterType() = 0;
    virtual int32_t     Size()         = 0;
};
#if REGION(非关联容器: StaticArray)
template<typename ClassT, typename T, size_t N>
class StaticArrayView : public ContainerView
{
public:
    static_assert(!std::is_reference_v<T>, "T must not be a reference, could be Ref<T> instead");
    using Iterator = typename StaticArray<T, N>::iterator;

    explicit StaticArrayView(StaticArray<T, N> ClassT::*container, const Type* outer) :
        container_(container), element_type_(TypeOf<T>()), outer_(outer)
    {
    }

    bool BeginIterate() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return false;
        }
        iter_     = (instance_->*container_).begin();
        size_     = N;
        iter_cnt_ = 1;
        return true;
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

    bool HasNext() override { return iter_cnt_ < size_; }

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
        Assert(LogCat::Reflection, index >= 0 && index < Size(), "Index out of range: index: {}, size: {}", index, Size());
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
        return N;
    }

private:
    ClassT* instance_ = nullptr;

    StaticArray<T, N> ClassT::*container_ = nullptr;

    Iterator    iter_;
    size_t      iter_cnt_ = 0;
    size_t      size_     = 0;
    const Type* element_type_{};
    const Type* outer_{};
};
#endif

#if REGION(非关联容器: Array List Set HashSet)
template<typename ClassT, typename T, template<typename...> typename Container>
class SequentialContainerView : public ContainerView
{
public:
    static_assert(!std::is_reference_v<T>, "T must not be a reference, could be Ref<T> instead");
    using Iterator = typename Container<T>::iterator;

    explicit SequentialContainerView(Container<T> ClassT::*container, const Type* outer) :
        container_(container), element_type_(TypeOf<T>()), outer_(outer)
    {
    }

    bool BeginIterate() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return false;
        }
        iter_     = (instance_->*container_).begin();
        size_     = (instance_->*container_).size();
        iter_cnt_ = 1;
        return true;
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

    bool HasNext() override { return iter_cnt_ < size_; }

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

    Container<T> ClassT::*container_ = nullptr;

    Iterator    iter_;
    size_t      iter_cnt_ = 0;
    size_t      size_     = 0;
    const Type* element_type_{};
    const Type* outer_{};
};
#endif



}   // namespace core
