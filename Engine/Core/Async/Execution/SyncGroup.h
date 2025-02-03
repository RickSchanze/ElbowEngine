//
// Created by Echo on 25-1-27.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/CoreGlobal.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Singleton/Singleton.h"
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

struct SyncGroupBase {
public:
  friend class SyncGroupManager;
  virtual ~SyncGroupBase() = default;
};

class SyncGroupManager : Singleton<SyncGroupManager> {
public:
  static void AddGroup(SyncGroupBase *group);

  static void RemoveGroup(SyncGroupBase *group);

private:
  List<SyncGroupBase *> groups_;
};

template <typename... AsyncResultHandleTypes> class SyncGroup : public SyncGroupBase {
public:
  using ResultType = typename MergeTuples<typename AsyncResultTrait<AsyncResultHandleTypes>::Type...>::type;
  using NestedTupleType = typename MergeTupleNested<typename AsyncResultTrait<AsyncResultHandleTypes>::Type...>::type;

  SyncGroup(AsyncResultHandleTypes &&...handles) {
    SetupOnCompleted(std::forward<AsyncResultHandleTypes>(handles)...,
                     std::make_index_sequence<sizeof...(AsyncResultHandleTypes)>());
  }

  ~SyncGroup() override = default;

  void OnCompleted(Function<void(const ResultType &)> &&callback) {
    PROFILE_SCOPE_AUTO;
    if (count_ == 0) {
      callback(FlattenTuple(result_));
      SyncGroupManager::RemoveGroup(this);
    } else {
      callback_ = callback;
    }
  }

private:
  template <UInt64... I> void SetupOnCompleted(AsyncResultHandleTypes &&...handles, std::index_sequence<I...>) {
    PROFILE_SCOPE_AUTO;
    (handles->OnCompleted([this](const typename AsyncResultTrait<AsyncResultHandleTypes>::Type &result) {
      std::get<I>(result_) = result;
      if (--count_ == 0) {
        if (callback_) {
          callback_(FlattenTuple(result_));
        }
        SyncGroupManager::RemoveGroup(this);
      }
    }),
     ...);
  }

  NestedTupleType result_;
  Function<void(const ResultType &)> callback_;
  UInt64 count_ = sizeof...(AsyncResultHandleTypes);
};

template <typename... AsyncResultHandleTypes> auto MakeSyncGroup(AsyncResultHandleTypes &...args) {
  auto *new_group = New<SyncGroup<AsyncResultHandleTypes...>>(std::forward<AsyncResultHandleTypes>(args)...);
  SyncGroupManager::AddGroup(new_group);
  return new_group;
}

} // namespace core::exec
