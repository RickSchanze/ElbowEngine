/**
 * @file Object.h
 * @author Echo
 * @Date 2024/4/1
 * @brief
 */

#pragma once

#include "Core/Async/Execution/StartAsync.h"
#include "Core/Base/EString.h"
#include "Core/Reflection/ITypeGetter.h"
#include "Core/Reflection/MetaInfoMacro.h"
#include "Core/Reflection/MetaInfoManager.h"

#include GEN_HEADER("Core.Object.generated.h")

namespace core {

using ObjectHandle = int32_t;

constexpr inline ObjectHandle INVALID_OBJECT_HANDLE = 0;

enum ENUM(Flag) ObjectFlagType {
  OFT_Persistent = 1 << 1, // 此对象需要持久化存储
  OFT_Actor = 1 << 2,
  OFT_Component = 1 << 3,
  OFT_Widget = 1 << 4,
};
using ObjectFlag = int32_t;

enum ENUM(Flag) ObjectStateFlagType {
  PendingKill,
};

using ObjectStateFlag = int32_t;

/**
 * Object不自动生成默认构造函数
 * TODO: Destroy
 */
class CLASS() Object : public ITypeGetter {
  GENERATED_CLASS(Object)
  friend struct ObjectPtrBase;
  friend struct ObjectRegistry;

public:
  explicit Object(ObjectFlag flag);

  Object();

  ~Object() override;

  void SetDisplayName(StringView display_name);

  void SetName(StringView name);

  [[nodiscard]] StringView GetName() const { return name_; }
  [[nodiscard]] StringView GetDisplayName() const { return display_name_; }

  void _SetObjectHandle(ObjectHandle handle) { handle_ = handle; }

protected:
  PROPERTY()
  ObjectHandle handle_ = 0;

  PROPERTY()
  ObjectFlag flags_ = 0;

  PROPERTY()
  ObjectStateFlag state_ = 0;

  PROPERTY()
  String name_{};

#if WITH_EDITOR
  PROPERTY()
  String display_name_{"空对象"};
#endif

private:
  // 此Object正在引用的对象, 用Array是因为可能会被同一个Object多次引用
  Array<ObjectHandle> referencing_;
  // 哪些Object引用了此Object
  Array<ObjectHandle> referenced_;

  void AddReferencing(ObjectHandle handle);
  void RemoveReferencing(ObjectHandle handle);
  void AddReferenced(ObjectHandle handle);
  void RemoveReferenced(ObjectHandle handle);

  void GenerateInstanceHandle();
  /**
   * 只是从ObjectArray中移除自身而不销毁
   */
  void UnregisterSelf() const;
  void RegisterSelf();
  void ResolveObjectPtr();

public:
  virtual void PostSerialized();
  virtual void PreSerialized() {}

  /**
   * 默认的实现中, 会遍历所有成员, 将由
   * ObjectPtr包裹的成语初始化
   */
  virtual void PostDeserialized();
  virtual void PreDeserialized() {}

  /**
   * 通过NewObject创建出来的对象不是Persistent对象, 不应该被序列化
   * 此时由此函数来生成ObjectHandle, 以及初始化ObjectPtr
   */
  virtual void OnCreated();

  [[nodiscard]] bool IsPendingKill() const { return state_ & PendingKill; }

  [[nodiscard]] ObjectHandle GetHandle() const { return handle_; }

  [[nodiscard]] bool IsPersistent() const { return flags_ & OFT_Persistent; }

  void InternalSetAssetHandle(ObjectHandle handle);

  // TODO: 这里是否应该返回一个sender?
  exec::AsyncResultHandle<ObjectHandle> PerformPersistentObjectLoadAsync();

  ObjectHandle PerformPersistentObjectLoad();
};

template <typename T> T *Cast(Object *obj) {
  auto obj_type = obj->GetType();
  if (auto t_type = TypeOf<T>(); t_type->IsDerivedFrom(obj_type)) {
    return reinterpret_cast<T *>(obj);
  }
  return nullptr;
}

template <typename T, typename... Args>
  requires std::is_base_of_v<Object, T>
T *NewObject(Args &&... args)
{
    T* obj = New<T>(Forward<Args>(args)...);
    obj->OnCreated();
    return obj;
}

}   // namespace core
