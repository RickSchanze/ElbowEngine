/**
 * @file ContainerView.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once

#include "Any.h"
#include "Base/CoreTypeDef.h"
#include "Log/Logger.h"
#include "Log/CoreLogCategory.h"
#include "CoreDef.h"

namespace core
{

enum class ContainerViewType
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

    virtual bool          BeginIterate()     = 0;
    virtual void          Next()             = 0;
    virtual void          EndIterate()       = 0;
    virtual bool          HasNext()          = 0;
    virtual void          SetInstance(void*) = 0;
    virtual ContainerViewType GetContainerType() = 0;
    virtual const Type*   GetOuterType()     = 0;
    virtual int32_t       Size()             = 0;
};

class SequentialContainerView : public ContainerView
{
public:
    virtual const Type* GetElementType()            = 0;
    virtual Any         GetCurrentElement()         = 0;
    virtual Any         GetElementAt(int32_t index) = 0;
    void                ForEach(const Function<void(Any)>& Func);
    void                ForEach(const Function<void(int, Any)>& Func);
};

class AssociativeContainerView : public ContainerView
{
public:
    virtual const Type* GetKeyType()                 = 0;
    virtual const Type* GetValueType()               = 0;
    virtual Any         GetCurrentKey()              = 0;
    virtual Any         GetCurrentValue()            = 0;
    virtual Any         GetElementAt(const Any& key) = 0;
    void                ForEach(const Function<void(Any, Any)>& Func);
};

#if REGION(非关联容器: StaticArray)
template<typename ClassT, typename T, size_t N>
class StaticArrayView : public SequentialContainerView
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

    ContainerViewType GetContainerType() override { return ContainerViewType::Sequential; }

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
class DynamicArrayView : public SequentialContainerView
{
public:
    static_assert(!std::is_reference_v<T>, "T must not be a reference, could be Ref<T> instead");
    using Iterator = typename Container<T>::iterator;

    explicit DynamicArrayView(Container<T> ClassT::*container, const Type* outer) : container_(container), element_type_(TypeOf<T>()), outer_(outer)
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

    ContainerViewType GetContainerType() override { return ContainerViewType::Sequential; }

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

#if REGION(关联容器: Map HashMap)
template<typename ClassT, typename K, typename V, template<typename...> typename Container>
class MapView : public AssociativeContainerView
{
    using key_type      = typename Container<K, V>::key_type;
    using mapped_type   = typename Container<K, V>::mapped_type;
    using value_type    = typename Container<K, V>::value_type;
    using iterator_type = typename Container<K, V>::iterator;

public:
    MapView(Container<K, V> ClassT::*container, const Type* outer) :
        container_(container), key_type_(TypeOf<K>()), value_type_(TypeOf<V>()), outer_(outer)
    {
    }

    bool BeginIterate() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return false;
        }
        size_     = Size();
        iter_     = (instance_->*container_).begin();
        iter_cnt_ = 1;
        return true;
    }

    void Next() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
        }
        ++iter_;
    }

    void EndIterate() override
    {
        iter_cnt_ = 0;
        size_     = 0;
        iter_     = {};
    }

    bool HasNext() override { return iter_cnt_ < size_; }

    void SetInstance(void* instance) override { instance_ = static_cast<ClassT*>(instance); }

    int32_t Size() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return 0;
        }
        return (instance_->*container_).size();
    }
    ContainerViewType GetContainerType() override { return ContainerViewType::Associative; }
    const Type*   GetOuterType() override { return outer_; }
    const Type*   GetKeyType() override { return key_type_; }
    const Type*   GetValueType() override { return value_type_; }

    Any GetCurrentKey() override
    {
        // key使用const ref
        return MakeRef(iter_->first);
    }

    Any GetCurrentValue() override { return MakeRef(iter_->second); }

    Any GetElementAt(const Any& key) override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
            return {};
        }
        std::expected<const K, AnyCastError> cast_op = any_cast<const K>(key);
        if (cast_op.has_value())
        {
            auto& key_value = cast_op.value();
            if ((instance_->*container_).find(key_value) != (instance_->*container_).end())
            {
                return MakeRef((instance_->*container_)[key_value]);
            }
        }
        return {};
    }

private:
    Container<K, V> ClassT::*container_ = nullptr;

    ClassT*       instance_ = nullptr;
    iterator_type iter_{};
    size_t        iter_cnt_ = 0;
    size_t        size_     = 0;
    const Type*   key_type_{};
    const Type*   value_type_{};
    const Type*   outer_{};
};
#endif

}   // namespace core