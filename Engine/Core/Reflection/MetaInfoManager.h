/**
 * @file MetaInfoManager.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once

#include "Base/CoreTypeDef.h"
#include "Base/EString.h"
#include "Singleton/Singleton.h"
namespace core
{
struct Type;

struct MetaDataRegisterer
{
    Type*      (*registerer)();
    StringView name;
};

class MetaInfoManager : public Singleton<MetaInfoManager>
{
public:
    MetaInfoManager();

    ~MetaInfoManager() override;

    void RegisterType(size_t type_hash);
    void RegisterTypeRegisterer(size_t type_hash, const MetaDataRegisterer& registerer);

    Type* GetType(size_t type_hash);
    Type* GetType(StringView type_name);

private:
    HashMap<size_t, Type*>              types_registered_;
    HashMap<size_t, MetaDataRegisterer> meta_data_registers_;
};

template<typename T>
const Type* TypeOf()
{
    size_t hash = typeid(T).hash_code();
    return MetaInfoManager::Get()->GetType(hash);
}
}   // namespace core
