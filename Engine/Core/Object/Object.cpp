/**
 * @file Object.cpp
 * @author Echo
 * @Date 2024/4/1
 * @brief
 */

#include "Object.h"

#include "Core/Async/Execution/Just.h"
#include "Core/Async/Execution/Then.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/Log/Logger.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/Archive.h"
#include "ObjectRegistry.h"
#include "PersistentObject.h"

#include GEN_HEADER("Core.Object.generated.h")

GENERATED_SOURCE()

using namespace core;
using namespace exec;

Object::Object(ObjectFlag flag) : flags_(flag) { GenerateInstanceHandle(); }

Object::Object() { GenerateInstanceHandle(); }

Object::~Object() {
  if (referenced_.empty())
    return;
  LOGGER.Warn(logcat::Core_Object, "对象{}被销毁时仍有{}个对象引用它", name_, referenced_.size());
  for (const auto &handle : referenced_) {
    Object *obj = ObjectManager::GetRegistry().GetObjectByHandle(handle);
    if (obj) {
      LOGGER.WarnFast(logcat::Core_Object, "{}: {}", handle, obj->name_);
    } else {
      LOGGER.WarnFast(logcat::Core_Object, "{}: {}", handle, "已失效");
    }
  }
}

void Object::SetDisplayName(StringView display_name) {
#if WITH_EDITOR
  display_name_ = display_name;
#endif
}

void Object::SetName(StringView name) { name_ = name; }

void Object::AddReferencing(ObjectHandle handle) { referencing_.push_back(handle); }

void Object::RemoveReferencing(ObjectHandle handle) { erase(referencing_, handle); }

void Object::AddReferenced(ObjectHandle handle) { referenced_.push_back(handle); }

void Object::RemoveReferenced(ObjectHandle handle) { erase(referenced_, handle); }

void Object::GenerateInstanceHandle() { handle_ = ObjectManager::GetRegistry().NextInstanceHandle(); }

void Object::UnregisterSelf() const { ObjectManager::GetRegistry().UnregisterHandle(GetHandle()); }

void Object::RegisterSelf() { ObjectManager::GetRegistry().RegisterObject(this); }

void Object::ResolveObjectPtr() {
  const auto type = GetType();
  const auto fields = type->GetFields();
  for (const auto &field : fields) {
    if (field->GetType() == TypeOf<ObjectPtrBase>()) {
      if (field->IsSequentialContainer()) {
      } else if (field->IsAssociativeContainer()) {
      } else {
        auto ptr = field->GetFieldPtr(this);
        static_cast<ObjectPtrBase *>(ptr)->SetOuter(handle_);
      }
    }
  }
}

ExecFuture<ObjectHandle> Object::PerformPersistentObjectLoadAsync() {
  if (!IsPersistent()) {
    LOGGER.Warn("Core.Object", "尝试加载非持久化对象, handle={}", GetHandle());
    return MakeExecFuture(0);
  }
  auto task = Just() | Then([this] {
                ((PersistentObject *)(this))->PerformLoad();
                return GetHandle();
              });
  auto f = ThreadManager::ScheduleFutureAsync(task);
  return ExecFuture<ObjectHandle>(Move(f));
}

ObjectHandle Object::PerformPersistentObjectLoad() {
  if (!IsPersistent())
    return INVALID_OBJECT_HANDLE;
  auto *persistent = static_cast<PersistentObject *>(this);
  persistent->PerformLoad();
  return GetHandle();
}

void Object::PostSerialized() {}

void Object::PostDeserialized() { RegisterSelf(); }

void Object::OnCreated() {
  RegisterSelf();
  ResolveObjectPtr();
}

static void FindObjPtr(const Type *t, void *field_ptr, ObjectHandle handle) {
  if (t == nullptr)
    return;
  const auto fields = t->GetFields();
  for (const auto &field : fields) {
    if (field->GetType() == TypeOf<ObjectPtrBase>()) {
      auto ptr = static_cast<ObjectPtrBase *>(field->GetFieldPtr(field_ptr));
      ptr->ModifyOuter(handle);
    } else {
      FindObjPtr(field->GetType(), field->GetFieldPtr(field_ptr), handle);
    }
  }
}

void Object::InternalSetAssetHandle(ObjectHandle handle) {
  // 此时自己的Object Handle 需要通知所有的ObjectPtr
  auto type = GetType();
  FindObjPtr(type, this, handle);
  UnregisterSelf();
  handle_ = handle;
  RegisterSelf();
}
