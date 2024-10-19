/**
 * @file Delegate.h
 * @author Echo 
 * @Date 24-10-19
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "CoreTypeTraits.h"


#include <limits>
#include <numeric>

struct DelegateID
{
    DelegateID() : id_(s_id_counter++) {}

    uint16_t GetID() const { return id_; }

    bool IsValid() const { return id_ != 0; }

    void Reset() { id_ = 0; }

    bool operator==(const DelegateID& other) const { return id_ == other.id_; }

    bool operator!=(const DelegateID& other) const { return id_ != other.id_; }

private:
    static inline uint16_t s_id_counter = 1;

    uint16_t id_ = 0;
};

template<>
struct std::hash<DelegateID>
{
    std::size_t operator()(const DelegateID& id) const noexcept { return std::hash<uint16_t>{}(id.GetID()); }
};

// TDelegate是一个带Id的TFunction wrapper
template<typename FuncType>
    requires std::is_invocable_v<FuncType>
struct Delegate
{
    using ReturnType    = ReturnTypeOf<FuncType>;
    using ArgumentTypes = ArgumentTypesOf<FuncType>;

    explicit Delegate(Function<FuncType>&& function) : function_(std::move(function)) {}

    DelegateID GetID() const { return id_; }

    void Reset()
    {
        function_.swap(Function<FuncType>{});
        id_.Reset();
    }

    bool IsValid() const { return id_.IsValid(); }

    ReturnType operator()(ArgumentTypes&&... args) const { return function_(std::forward<ArgumentTypes>(args)...); }

private:
    Function<FuncType> function_;
    DelegateID         id_;
};
