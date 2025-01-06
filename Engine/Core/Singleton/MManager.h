/**
 * @file MManager.h
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"
#include "Core/CoreGlobal.h"
#include "Singleton.h"


#include <cstdint>



namespace core
{
enum class ManagerLevel
{
    Top,
    Middle,
    Bottom,
    Count,
};

class ManagerBase
{
public:
    virtual ~ManagerBase() = default;

    [[nodiscard]] virtual ManagerLevel GetLevel() const = 0;
    virtual void                       Startup() {}
    virtual void                       Shutdown() {}
    [[nodiscard]] virtual StringView   GetName() const = 0;
};

// Manager of Manager
class MManager : public Singleton<MManager>
{
public:
    MManager();
    void RegisterManager(ManagerBase* manager);
    void Startup();
    void Shutdown() const;

private:
    Array<ManagerBase*> managers_[static_cast<int32_t>(ManagerLevel::Count)] = {};
};

template <typename T>
class Manager : public ManagerBase
{
public:
    static T* Get()
    {
        static T* instance;
        if (!instance)
        {
            instance = New<T>();
        }
        MManager::GetByRef().RegisterManager(instance);
        return instance;
    }

    static T& GetByRef() { return *Get(); }
};

}   // namespace core
