//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Any.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/TypeAlias.hpp"

enum class ContainerViewType {
    Sequential,
    Associative,
    Count,
};

/**
 * 用于反射容器的类, 此类可以迭代, 可以获取元素
 * 但是(暂时)无法增大、减小元素
 * TODO: 完整的增删改查接口
 */
class ContainerView {
public:
    virtual ~ContainerView() = default;

    virtual bool BeginIterate() = 0;
    virtual void Next() = 0;
    virtual void EndIterate() = 0;
    virtual bool HasNext() = 0;
    virtual void SetInstance(void *) = 0;
    virtual ContainerViewType GetContainerType() = 0;
    virtual const Type *GetOuterType() = 0;
    virtual UInt64 GetSize() = 0;
    virtual void Clear() = 0;
};

class SequentialContainerView : public ContainerView {
public:
    virtual const Type *GetElementType() = 0;
    virtual Any GetCurrentElement() = 0;
    virtual Any GetElementAt(Int32 index) = 0;
    virtual bool Add(Any element) = 0;
    void ForEach(const Function<void(Any)> &Func);
    void ForEach(const Function<void(int, Any)> &Func);
};

class AssociativeContainerView : public ContainerView {
public:
    virtual const Type *GetKeyType() = 0;
    virtual const Type *GetValueType() = 0;
    virtual Any GetCurrentKey() = 0;
    virtual Any GetCurrentValue() = 0;
    virtual Any GetElementAt(const Any &key) = 0;
    virtual bool Add(Any key, Any value) = 0;
    void ForEach(const Function<void(Any, Any)> &Func);
};

template<typename ClassT, typename T, template<typename...> typename Container>
class DynamicArrayView : public SequentialContainerView {
public:
    static_assert(!NTraits::IsReference<T>, "T must not be a reference, could be Ref<T> instead");
    using Iterator = typename Container<T>::iterator;

    explicit DynamicArrayView(Container<T> ClassT::* container, const Type* outer) : container_(container), outer_(outer)
    {
        element_type_ = TypeOf<T>();
    }

    virtual bool BeginIterate() override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";
            return false;
        }
        const size_t size = (instance_->*container_).Count();
        if (size == 0)
            return false;
        iter_ = (instance_->*container_).begin();
        size_ = size;
        iter_cnt_ = 1;
        return true;
    }

    virtual bool Add(Any element) override
    {
        if (element.GetType() != element_type_)
        {
            return false;
        }
        if constexpr (NTraits::IsSame<Container<T>, Array<T>>)
        {
            (instance_->*container_).Add(*element.As<T>());
            return true;
        }
        else
        {
            return (instance_->*container_).Insert(*element.As<T>()).second;
        }
    }

    virtual void EndIterate() override
    {
        iter_ = {};
        size_ = 0;
        iter_cnt_ = 0;
    }

    virtual void Next() override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";
            return;
        }
        ++iter_;
        ++iter_cnt_;
    }

    virtual bool HasNext() override
    {
        return iter_cnt_ < size_;
    }

    virtual void SetInstance(void* instance) override
    {
        instance_ = static_cast<ClassT*>(instance);
    }

    virtual Any GetCurrentElement() override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";
            return {};
        }
        return {AddressOf(*iter_), element_type_};
    }

    virtual ContainerViewType GetContainerType() override
    {
        return ContainerViewType::Sequential;
    }

    virtual const Type* GetElementType() override
    {
        return element_type_;
    }
    virtual const Type* GetOuterType() override
    {
        return outer_;
    }

    virtual Any GetElementAt(Int32 index) override
    {
        Iterator it = (instance_->*container_).begin();
        for (Int32 i = 0; i < index; ++i)
        {
            ++it;
        }
        return {AddressOf(*it), element_type_};
    }

    virtual UInt64 GetSize() override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";
            return 0;
        }
        return (instance_->*container_).Count();
    }

    virtual void Clear() override {
        if (!instance_) {
            Log(Error) << "未设置Instance";
            return;
        }
        (instance_->*container_).Clear();
    }

private:
    ClassT *instance_ = nullptr;

    Container<T> ClassT::*container_ = nullptr;

    Iterator iter_;
    size_t iter_cnt_ = 0;
    size_t size_ = 0;
    const Type *element_type_{};
    const Type *outer_{};
};

template<typename ClassT, typename K, typename V, template<typename...> typename Container>
class MapView : public AssociativeContainerView {
    using key_type = typename Container<K, V>::key_type;
    using mapped_type = typename Container<K, V>::mapped_type;
    using value_type = typename Container<K, V>::value_type;
    using iterator_type = typename Container<K, V>::iterator;

public:
    MapView(Container<K, V> ClassT::* container, const Type* outer) : container_(container), key_type_(TypeOf<K>()), outer_(outer)
    {
        value_type_ = TypeOf<V>();
    }

    virtual bool BeginIterate() override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";
            return false;
        }
        if (GetSize() <= 0)
            return false;
        size_ = GetSize();
        iter_ = (instance_->*container_).begin();
        iter_cnt_ = 1;
        return true;
    }

    virtual void Next() override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";
        }
        ++iter_;
        ++iter_cnt_;
    }

    virtual void EndIterate() override
    {
        iter_cnt_ = 0;
        size_ = 0;
        iter_ = {};
    }

    virtual bool HasNext() override
    {
        return iter_cnt_ < size_;
    }

    virtual void SetInstance(void* instance) override
    {
        instance_ = static_cast<ClassT*>(instance);
    }

    virtual UInt64 GetSize() override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";

            return 0;
        }
        return (instance_->*container_).Count();
    }

    virtual ContainerViewType GetContainerType() override
    {
        return ContainerViewType::Associative;
    }
    virtual const Type* GetOuterType() override
    {
        return outer_;
    }
    virtual const Type* GetKeyType() override
    {
        return key_type_;
    }
    virtual const Type* GetValueType() override
    {
        return value_type_;
    }

    virtual Any GetCurrentKey() override
    {
        // key使用const ref
        return {AddressOf(iter_->first), key_type_};
    }

    virtual Any GetCurrentValue() override
    {
        return {AddressOf(iter_->second), value_type_};
    }

    virtual Any GetElementAt(const Any& key) override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";

            return {};
        }
        auto cast_op = key.AsCopy<K>();
        if (cast_op)
        {
            auto& key_value = *cast_op;
            if ((instance_->*container_).Find(key_value) != (instance_->*container_).end())
            {
                auto& it = (instance_->*container_)[key_value];
                return {AddressOf(it), value_type_};
            }
        }
        return {};
    }

    virtual bool Add(Any key, Any value) override
    {
        if (!instance_)
        {
            Log(Error) << "未设置Instance";

            return {};
        }
        auto key_op = key.AsCopy<K>();
        auto value_op = value.AsCopy<V>();
        if (key_op && value_op)
        {
            if ((instance_->*container_).Find(*key_op) == (instance_->*container_).end())
            {
                (instance_->*container_)[*key_op] = *value_op;
                return true;
            }
        }
        return false;
    }

    virtual void Clear() override {
        if (!instance_) {
            Log(Error) << "未设置Instance";

            return;
        }
        (instance_->*container_).Clear();
    }

private:
    Container<K, V> ClassT::*container_ = nullptr;

    ClassT *instance_ = nullptr;
    iterator_type iter_{};
    size_t iter_cnt_ = 0;
    size_t size_ = 0;
    const Type *key_type_{};
    const Type *value_type_{};
    const Type *outer_{};
};
