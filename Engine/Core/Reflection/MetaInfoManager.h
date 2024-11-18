/**
 * @file MetaInfoManager.h
 * @author Echo
 * @Date 24-10-27
 * @brief
 */

#pragma once

#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"
#include "Core/Singleton/Singleton.h"

namespace core
{
class ITypeGetter;
}
namespace core
{
struct RTTITypeInfo;
}
template <>
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

    template <typename T>
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
    void RegisterType(RTTITypeInfo type_info, Type* type);

    Type* GetType(const RTTITypeInfo& type_info);

private:
    HashMap<RTTITypeInfo, Type*>              types_registered_;
    HashMap<RTTITypeInfo, MetaDataRegisterer> meta_data_registers_;
};

template <typename T>
const Type* TypeOf()
{
    using OriginalT   = std::remove_cvref_t<T>;
    RTTITypeInfo info = {typeid(OriginalT).name(), typeid(OriginalT).hash_code()};
    return MetaInfoManager::Get()->GetType(info);
}
}   // namespace core
