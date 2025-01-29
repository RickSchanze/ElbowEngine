/**
 * @file CoreConfig.cpp
 * @author Echo
 * @Date 24-11-16
 * @brief
 */

#include "CoreConfig.h"
#include "Core/Async/ThreadManager.h"

#include GEN_HEADER("Core.CoreConfig.generated.h")

static void Z_MetaInfo_Registration_Func();
namespace {
struct Z_MetaInfo_Registration {
  Z_MetaInfo_Registration() {
    Z_MetaInfo_Registration_Func();
  }
};

static const Z_MetaInfo_Registration Z_meta_info_registration1;
} // namespace
void Z_MetaInfo_Registration_Func() {
  using namespace core;
  core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Version>(),
                                                       &core::Version::REFLECTION_Register_Version_Registerer);
  core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Version>(), &core::Version::ConstructAt,
                                             &core::Version::DestroyAt);
  core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::CoreConfig>(),
                                                       &core::CoreConfig::REFLECTION_Register_CoreConfig_Registerer);
  core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::CoreConfig>(), &core::CoreConfig::ConstructAt,
                                             &core::CoreConfig::DestroyAt);
}
core::Type *core::Version::REFLECTION_Register_Version_Registerer() {
  using namespace core;
  Type *type = Type::Create<core::Version>("core.Version")->SetAttribute(Type::Trivial);
  type->Internal_RegisterField("major", &core::Version::major, __builtin_offsetof(core::Version, major));
  type->Internal_RegisterField("minor", &core::Version::minor, __builtin_offsetof(core::Version, minor));
  type->Internal_RegisterField("patch", &core::Version::patch, __builtin_offsetof(core::Version, patch));
  return type;
}
core::Type *core::CoreConfig::REFLECTION_Register_CoreConfig_Registerer() {
  using namespace core;
  Type *type = Type::Create<core::CoreConfig>("core.CoreConfig")
                   ->Internal_AddParent(TypeOf<core::IConfig>())
                   ->SetAttribute(core::Type::ValueAttribute::Config, "Config/Core/Core.cfg")
                   ->SetAttribute(core::Type::ValueAttribute::Category, "Core");
  type->Internal_RegisterField("app_name", &core::CoreConfig::app_name, __builtin_offsetof(core::CoreConfig, app_name))
      ->SetComment("应用的名称");
  type->Internal_RegisterField("app_version", &core::CoreConfig::app_version,
                               __builtin_offsetof(core::CoreConfig, app_version))
      ->SetComment("应用版本号");
  type->Internal_RegisterField("thread_slot_count", &core::CoreConfig::thread_slot_count,
                               __builtin_offsetof(core::CoreConfig, thread_slot_count))
      ->SetComment("各ThreadSlot对应的线程数")
      ->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Thread");
  type->Internal_RegisterField("enable_persistent_load_multithread",
                               &core::CoreConfig::enable_persistent_load_multithread,
                               __builtin_offsetof(core::CoreConfig, enable_persistent_load_multithread))
      ->SetAttribute(core::FieldInfo::ValueAttribute::Label, "启用多线程持久化对象加载");
  return type;
}

bool core::CoreConfig::IsMultiThreadPersistentLoadEnabled() const {
  auto &thread_map = GetThreadSlotCountMap();
  auto find = thread_map.find(ThreadSlot::Resource);
  if (find != thread_map.end()) {
    return find->second >= 1 && GetEnablePersistentLoadMultiThread();
    }
    return false;
}
