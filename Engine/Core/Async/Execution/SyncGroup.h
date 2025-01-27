//
// Created by Echo on 25-1-27.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Profiler/ProfileMacro.h"
#include "StartAsync.h"

#include <mutex>

namespace core::exec {
template <typename T> struct _SharedPtrTrait {
  using Type = void;
};

template <typename T> struct _SharedPtrTrait<SharedPtr<T>> {
  using Type = T;
};

template <typename T> struct AsyncResultTrait {
  using Type = typename _SharedPtrTrait<T>::Type;
};

template <typename... T> struct AsyncResultTrait<SharedPtr<AsyncResult<std::tuple<T...>>>> {
  using Type = std::tuple<T...>;
};

template <typename... AsyncResultHandleTypes> class SyncGroup {
public:
  using ResultType = typename MergeTuples<typename AsyncResultTrait<AsyncResultHandleTypes>::Type...>::type;
  using NestedTupleType = typename MergeTupleNested<typename AsyncResultTrait<AsyncResultHandleTypes>::Type...>::type;

  SyncGroup(AsyncResultHandleTypes &&...handles) {
    SetupOnCompleted(std::forward<AsyncResultHandleTypes>(handles)...,
                     std::make_index_sequence<sizeof...(AsyncResultHandleTypes)>());
  }

  void OnCompleted(Function<void(const ResultType &)> callback) {
    if (count_ == 0) {
      callback(FlattenTuple(result_));
    } else {
      callback_ = callback;
    }
  }

private:
  template <UInt64... I> void SetupOnCompleted(AsyncResultHandleTypes &&...handles, std::index_sequence<I...>) {
    (handles->OnCompleted([this](const typename AsyncResultTrait<AsyncResultHandleTypes>::Type &result) {
      std::get<I>(result_) = result;
      if (--count_ == 0) {
        callback_(FlattenTuple(result_));
      }
    }),
     ...);
  }

  NestedTupleType result_;
  Function<void(const ResultType &)> callback_;
  UInt64 count_ = sizeof...(AsyncResultHandleTypes);
};

template <typename... AsyncResultHandleTypes> auto MakeSyncGroup(AsyncResultHandleTypes &&...args) {
  return SyncGroup<AsyncResultHandleTypes...>(std::forward<AsyncResultHandleTypes>(args)...);
}

} // namespace core::exec
