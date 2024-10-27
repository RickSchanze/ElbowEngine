/**
 * @file MetaInfoManager.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once

#include "Singleton/Singleton.h"
#include "Base/CoreTypeDef.h"
namespace core
{
struct Type;
typedef Type* (*MetaDataRegisterer)();

class MetaInfoManager : public Singleton<MetaInfoManager>
{
public:
    MetaInfoManager();

    ~MetaInfoManager() override;

    void RegisterType(size_t type_hash);
    void RegisterTypeRegisterer(size_t type_hash, MetaDataRegisterer registerer);

    Type* GetType(size_t type_hash);

private:
    HashMap<size_t, Type*>              types_registered_;
    HashMap<size_t, MetaDataRegisterer> meta_data_registers_;
};

template<typename T>
const Type& TypeOf()
{
    size_t hash = typeid(T).hash_code();
    return *MetaInfoManager::Get()->GetType(hash);
}
}   // namespace core
