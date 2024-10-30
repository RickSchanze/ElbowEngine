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
struct RTTITypeInfo;
}
template<>
struct ::std::hash<core::RTTITypeInfo>
{
    size_t operator()(const core::RTTITypeInfo& type) const noexcept;
};

namespace core
{
struct Type;

typedef Type* (*MetaDataRegisterer)();

struct RTTITypeInfo
{
    StringView name;
    size_t     hash_code{};

    template<typename T>
    static RTTITypeInfo Create()
    {
        return {typeid(T).name(), typeid(T).hash_code()};
    }

    bool operator==(const RTTITypeInfo& o) const { return hash_code == o.hash_code; }
};

class MetaInfoManager : public Singleton<MetaInfoManager>
{
public:
    MetaInfoManager();

    ~MetaInfoManager() override;

    void RegisterType(RTTITypeInfo type_info);
    void RegisterTypeRegisterer(RTTITypeInfo type_info, MetaDataRegisterer registerer);

    Type* GetType(const RTTITypeInfo& type_info);

private:
    HashMap<RTTITypeInfo, Type*>              types_registered_;
    HashMap<RTTITypeInfo, MetaDataRegisterer> meta_data_registers_;
};

template<typename T>
const Type* TypeOf()
{
    RTTITypeInfo info = {typeid(T).name(), typeid(T).hash_code()};
    return MetaInfoManager::Get()->GetType(info);
}
}   // namespace core


