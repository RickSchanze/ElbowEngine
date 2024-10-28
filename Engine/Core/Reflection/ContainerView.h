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
class ContainerView
{
public:
    virtual ~ContainerView() = default;

    virtual void BeginIterate()      = 0;
    virtual void Next()              = 0;
    virtual bool HasNext()           = 0;
    virtual void SetInstance(void*)  = 0;
    virtual Any  GetCurrentElement() = 0;

protected:
    std::any iterator_;
};

template<typename ClassT, typename T>
class SequenceContainerView : public ContainerView
{
public:
    void BeginIterate() override
    {
        // TODO 使用Assert替代
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
        }
        iterator_ = (instance_->*container_).begin();
    }

    void Next() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
        }
        if (HasNext())
        {
            auto iter = any_cast<typename Array<T>::iterator>(iterator_);
            ++iter;
            iterator_ = iter;
        }
    }

    bool HasNext() override
    {
        if (!instance_)
        {
            LOGGER.Error(LogCat::Reflection, "未设置Instance");
        }
        auto iter = any_cast<typename Array<T>::iterator>(iterator_);
        if (iter != (instance_->*container_).end())
        {
            return true;
        }
        return false;
    }

    void SetInstance(void* instance) override { instance_ = static_cast<ClassT*>(instance); }

    Any GetCurrentElement() override
    {
        auto iter = any_cast<typename Array<T>::iterator>(iterator_);
        return MakeRef(*iter);
    }

private:
    ClassT*  instance_           = nullptr;
    Array<T> ClassT::*container_ = nullptr;
};
}   // namespace core
